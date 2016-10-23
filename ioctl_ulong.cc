#include <errno.h>
#include <sys/ioctl.h>
#include "nan.h"

#define IsArgType(arg, Type) info[arg]->Is##Type()

#define ArgType(arg, Type, msg) \
if(!IsArgType(arg, Type)) { \
    return Nan::ThrowTypeError(msg); \
}

#define ArgToValue(arg, Type) info[arg]->Type##Value()

#define SetProperty(prop, Type, type, value) Nan::Set(return_value, Nan::New<v8::String>(prop).ToLocalChecked(), Nan::New<v8::Type>(static_cast<type>(value)))

NAN_METHOD(IoctlUlong) {
    Nan::HandleScope scope;
    v8::Local<v8::Object> return_value = Nan::New<v8::Object>();
    long ioctl_return;
    unsigned long argument = 0;
    ArgType(0, Int32, "First Argument Must be an Integer (valid file discriptor)")
    ArgType(1, Uint32, "Second Argument Must be an Integer (uint32_t) (valid ioctl)")
    if(IsArgType(2, Uint32)) {
        argument = ArgToValue(2, Uint32);
    }
    ioctl_return = ioctl(ArgToValue(0, Int32), ArgToValue(1, Uint32), &argument);
    if(ioctl_return < 0) {
         return Nan::ThrowError(Nan::ErrnoException(errno, "ioctl"));
    }
    SetProperty("ioctl", Int32, int32_t, ioctl_return);
    SetProperty("data", Uint32, uint32_t, argument);
    info.GetReturnValue().Set(return_value);
}

void InitAll(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
    Nan::SetMethod(module, "exports", IoctlUlong);
}

NODE_MODULE(ioctl, InitAll)