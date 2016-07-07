# ioctl-ulong

unux'es ioctl addon wrapper working with unsigned long (both at user and kernel level, uses implicit cast from uit32_t to unsigned long so the limit is 4294967295 or 2^32)

# Installation

Install with npm:

```bash
$ npm install ioctl-ulong
```

# Purpose

ioctl-ulong it's not just another ioctl C++ addon wrapper for Node.js. It's not even fully campatible ioctl C++ addon wrapper. It's only purpose is to transfer largest posible unsigned integers from Node.js to to kernel (for now this are uint32_t) and to do it fast as possible. It dosen't provide the functionality to transfer struct like data like ioctl wrapper found at npm (https://www.npmjs.com/package/ioctl). So if you want to transfer more data at once use ioctl instead or do bit shifting operations like in the example. Bit shifting operations are required some times like intensive use of integer and floats in kernel module where switching context from integer to floating mode cpu do costs performence. Because of uint32_t beeing the limit the largest possible number wich will correctly be transfered to your driver is 4294967295 or 2^32.

ioctl-ulong also both provides information provided with pass by value and pass by reference from the kernel. Wich beeing said ioctl-ulong returns object with two fields {ioctl, data}, ioctl field holding the return value from making a successful syscall to ioctl and data which by default is 0, if third argument is provided data holds it's value if no value passed by reference is provided by the kernel else it's the value passed by reference is provided from the kernel data holds that value (More about that in the example).

In order ioctl-ulong to have best performence this is the algorithm it uses:

1. It checks if fd can be used as int32_t if it can't it throws TypeError.
2. It checks if ioctl_number can be used as uint32_t if it can't it throws TypeError.
3. It checks if argument isn't provided ioctl-ulong will use 0 by default wich will not afect _IO ioctl numbers since in kernel implementations of unlocked_ioctl and compact_ioctl both accepts unsigned long wich in fact is and should be interpreted as user space address from wich value can be recived with syscall to get_user and if you don't handle this pointer in your ioctl implementation it dosen't metter at all (as long as you don't use get_user ot put_user or any of <linux/uaccess.h> functions you are OK) else if argument is provided and it's value can be used as uitn32_t it's value is cast with implicit cast to unsigned long and a pointer to it is send to the kernel from wich you can get the value by syscalling get_user(value, (unsigned long __user*)argument) (more about why implicit cast is needed in FAQ section).
4. It resets errno
5. It makes a sys call to ioctl
6. If ioctl call was unsuccessful it's return value is less than 0 it throws a ErrnoException with information about the error else if the call was successful it returns an object { ioctl: ioctl_return_value, data: 0 | argument | data_from_kernel }

In fact this is ioctl-ulong source code:

```c++
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
```

Also ioctl-ulong is TypeScript ready, it ships with definition file.

If you still need to send more complex data to your device driver you can install ioct.

## To install ioctl via npm do:

```bash
npm install ioctl
```

# Usage

## ioctl(fd, ioctl_number, argumet?)

### fd: opened file descriptor (int32_t)

### ioctl_number: ioctl number to be trasfered to your driver (uint32_t)

### argument: optional number to be transfered to your driver as third argument (uint32_t)

# Example

# Will be added in the next 2 hours!!!

## TypeScript

```typescript
// index.d.ts
interface Ioctl {
    ioctl: number;
    data: number;
}

declare function ioctl(fd: number, command: number, argument?: number): Ioctl;

export { Ioctl, ioctl };

# FAQ

# Q: Why implicit cast from uint32-t to unsinged long is needed ?

A: You cann't be sure if user mode uint32_t provided by compiler you are using matches kernel defention, by value they may amtch but when it comes to pointers the answer is probably they don't. Test if you don't belive me even gcc's definitions may vary from version to version after all libc is just a program ...

## Q: Why are you natively supporting TypeScript ?

A: Well as a kernel hacker and developer who is making JavaScript MVC** IoT interfaces and some Node.js addons I'm using C and C++ alot and having staticly typed code is how big projects are done right. Also I like TypeScript it's the first good thing in Microsoft I see ... But more about me in About me section below.

# About me

## Q: Who I'm ?

A: I'm Ivo Stratev and I'm 19 years old who just finished hihg school at Technological school "Electronic Systems" at the Technical University - Sofia with 5.85 of possible 6.00.

There I've learn to code. I'm self taught in kernel programming currenlty only under Linux. I'm doing alot of IoT and for the interfaces I used to use Angular.js but now I'm using React and the new version of Angular (known as Angular 2) also using Node.js (express and Hapi) for middle layer in the communication with the device drivers I make.

I haven't always been staticly typed fan I used to do alot of Ruby, Python and CoffeeScript but after my last big project which at the end was more than 10K lines of code I realised that static type do pay of so I recommend every JavaScript programmer to use TypeScript (even if I my self hate Microsoft and what they have done ... mainly for IE, any way there are two good things created by them: AJAX and TypeScript).
```
