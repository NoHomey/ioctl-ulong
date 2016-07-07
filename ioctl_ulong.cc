#include <errno.h>
#include <sys/ioctl.h>
#include "nan.h"

NAN_METHOD(IoctlUlong) {
    Nan::HandleScope scope;
    v8::Local<v8::Object> return_value = Nan::New<v8::Object>();
    long ioctl_return;
    unsigned long argument = 0;
    if(!info[0]->IsInt32()) {
        return Nan::ThrowTypeError("First Argument Must be an Integer (valid file discriptor)");
    }
    if (!info[1]->IsUint32()) {
        return Nan::ThrowTypeError("Second Argument Must be an Integer (uint32_t) (valid ioct)");
    }
    if(!info[2]->IsUndefined() && info[1]->IsUint32()) {
        argument = info[2]->Uint32Value();
    }
    errno = 0;
    ioctl_return = ioctl(info[0]->Int32Value(), info[1]->Uint32Value(), &argument);
    if(ioctl_return < 0) {
         return Nan::ThrowError(Nan::ErrnoException(errno, "ioctl", strerror(errno)));
    }
    Nan::Set(return_value, Nan::New<v8::String>("ioctl").ToLocalChecked(), Nan::New<v8::Int32>(static_cast<int32_t>(ioctl_return)));
    Nan::Set(return_value, Nan::New<v8::String>("data").ToLocalChecked(), Nan::New<v8::Uint32>(static_cast<uint32_t>(argument)));
    info.GetReturnValue().Set(return_value);
}

void InitAll(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
    Nan::SetMethod(module, "exports", IoctlUlong);
}

NODE_MODULE(ioctl, InitAll)
