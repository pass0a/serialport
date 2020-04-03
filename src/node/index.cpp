#include <napi.h>
#include "serialport.h"
#include <set>
using namespace Napi;

class SerialWorker : public AsyncProgressWorker<SPMsg> {
public:
    SerialWorker(Function& callback, SerialPortOption opts)
        : AsyncProgressWorker(callback), err(0), needClose(0), opts(opts) {
        gp.open(opts);
    }

    ~SerialWorker() {
        gp.close();
    }
    // This code will be executed on the worker thread
    void Execute(const ExecutionProgress& progress) {
        // Need to simulate cpu heavy task
        SPMsg msg;
        std::unique_lock<std::mutex> lock(_cvm);
        msg.data.resize(opts.capacity);
        while (!needClose)
        {
            err = gp.read(msg);
            if (!err && msg.len) {
                progress.Send(&msg, 1);
                _cv.wait(lock);
            }
        }
    }

    void OnOK() {
        HandleScope scope(Env());
    }

    void OnProgress(const SPMsg* msg, size_t  count) {
        if (msg && count) {
            if (count > 1) { printf("has more!!!!"); }
            HandleScope scope(Env());
            Callback().Call({ Buffer<BYTE>::New(Env(),const_cast<BYTE*>(msg->data.data()),msg->len) });
        }
        _cv.notify_one();
    }
    void close() {
        needClose = true; gp.close();
    }
    int send(BYTE* buf, DWORD len) {
        gp.write(buf, len);
        return 0;
    }
private:
    SerialPortOption opts;
    SerialPort gp;
    bool needClose;
    int err;
    std::condition_variable _cv;
    std::mutex _cvm;
};
std::set<SerialWorker*> glist;
int getNumber(Object& obj, const char* key, int dv) {
    return obj.Has(key) ? obj.Get(key).ToNumber() : dv;
}
Value serialport_open(const CallbackInfo& info) {
    int ret = -1;
    auto obj = info[1].ToObject();
    SerialPortOption opts;
    opts.baud = getNumber(obj, "baud_rate", CBR_115200);
    opts.byteSize = getNumber(obj, "character_size", 0x08);
    opts.parity = getNumber(obj, "parity", NOPARITY);
    opts.stopBits = getNumber(obj, "stop_bits", ONESTOPBIT);
    opts.capacity = getNumber(obj, "capacity", 0x8000);
    opts.name = info[0].ToString();
    Function cb = info[2].As<Function>();

    SerialWorker *wk = new SerialWorker(cb, opts);
    wk->Queue();
    glist.insert(wk);
    return External<SerialWorker>::New(info.Env(), wk);
}
Value serialport_close(const CallbackInfo& info) {
    // We need to validate the arguments here
    if (info[0].IsExternal()) {
        auto wk = info[0].As<External<SerialWorker>>().Data();
        if (glist.find(wk) != glist.end())wk->close();
    }
    return info.Env().Undefined();
}
Value serialport_write(const CallbackInfo& info) {
    // We need to validate the arguments here
    if (info[0].IsExternal() && info[1].IsBuffer()) {
        auto wk = info[0].As<External<SerialWorker>>().Data();
        auto buf = info[1].As<Buffer<BYTE>>();
        auto len = buf.ByteLength();
        auto offset = buf.ByteOffset();
        if (glist.find(wk) != glist.end())wk->send(buf.Data(), buf.ByteLength());
    }
    return Number::New(info.Env(), 0);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "open"),
        Napi::Function::New(env, serialport_open));
    exports.Set(Napi::String::New(env, "close"),
        Napi::Function::New(env, serialport_close));
    exports.Set(Napi::String::New(env, "write"),
        Napi::Function::New(env, serialport_write));
    return exports;
}

NODE_API_MODULE(cvip, Init)