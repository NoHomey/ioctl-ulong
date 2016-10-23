# ioctl-ulong

unix'es ioctl addon wrapper working with unsigned long (both at user and kernel level, uses implicit cast from uit32_t to unsigned long so the limit is 4294967295 or 2^32)

[![npm version](https://badge.fury.io/js/ioctl-ulong.svg)](https://badge.fury.io/js/ioctl-ulong)
[![license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/NoHomey/ioctl-ulong)
![TypeScript](https://img.shields.io/badge/%3C%20%2F%3E-TypeScript-blue.svg)
![Typings](https://img.shields.io/badge/typings-%E2%9C%93-brightgreen.svg)

# Installation

Install with npm:

```bash
$ npm install ioctl-ulong
```

[![NPM](https://nodei.co/npm/ioctl-ulong.png?downloads=true&stars=true)](https://nodei.co/npm/ioctl-ulong/)

# Purpose

ioctl-ulong it's not just another ioctl C++ addon wrapper for Node.js. It's not even fully campatible ioctl C++ addon wrapper. It's only purpose is to transfer largest posible unsigned integers from Node.js to to kernel (for now this are uint32_t) and to do it fast as possible. It dosen't provide the functionality to transfer struct like data like ioctl wrapper found at npm (https://www.npmjs.com/package/ioctl). So if you want to transfer more data at once use ioctl instead or do bit shifting operations like in the example. Bit shifting operations are required some times like mixed intensive use of both integer and float operations in kernel module where switching context from integer to floating mode cpu do costs performence. Because of uint32_t beeing the limit the largest possible number which will correctly be transfered to your driver is 4294967295 or 2^32.

ioctl-ulong also both returns information provided with pass by value and pass by reference from the kernel. Wich beeing said ioctl-ulong returns object with two fields {ioctl, data}, ioctl field holding the return value from making a successful syscall to ioctl and data which by default is 0, if third argument is provided data holds it's value if no value passed by reference is provided by the kernel else it's the value passed by reference is provided from the kernel data holds that value (More about that in the example).

In order ioctl-ulong to have best performence this is the algorithm it uses:

1. It checks if fd can be used as int32_t if it can't it throws TypeError.
2. It checks if ioctl_number can be used as uint32_t if it can't it throws TypeError.
3. It checks if argument isn't provided ioctl-ulong will use 0 by default wich will not afect _IO ioctl numbers since in kernel implementations of unlocked_ioctl and compact_ioctl both accepts unsigned long wich in fact is and should be interpreted as user space address from wich value can be recived with syscall to get_user and if you don't handle this pointer in your ioctl implementation it dosen't metter at all (as long as you don't use get_user ot put_user or any of <linux/uaccess.h> functions you are OK) else if argument is provided and it's value can be used as uitn32_t it's value is cast with implicit cast to unsigned long and a pointer to it is send to the kernel from wich you can get the value by syscalling get_user(value, (unsigned long __user*)argument) (more about why implicit cast is needed in FAQ section).
4. It resets errno.
5. It makes a sys call to ioctl.
6. If ioctl call was unsuccessful it's return value is less than 0 it throws a ErrnoException with information about the error else if the call was successful it returns an object { ioctl: ioctl_return_value, data: 0 | argument | data_from_kernel }.

Also ioctl-ulong is TypeScript ready, it ships with definition file.

If you still need to send more complex data to your device driver you can install ioct.

## To install ioctl via npm do:

```bash
npm install ioctl
```

# Usage

## ioctl(fd: number, ioctl_number: number, argumet?: number): { ioctl: number, data: number }

### fd: opened file descriptor (int32_t)

### ioctl_number: ioctl number to be trasfered to your driver (uint32_t)

### argument: optional number to be transfered to your driver as third argument (uint32_t)

# Example

It is in TypeScript but pure JavaScript programmers shouldn't hvae problem with it just ignore type annotaions (It is a langauge of choice for me).

For full working example code visit: (https://github.com/NoHomey/nodejs-ioctl-example).
Example uses the following npm packages: ioctl-enum and open-ioctl go check them out if you are interested in them.

Consider the follwing ioctl numbers:

```c
// ioctl_example.h
#define IOCTL_EXAMPLE_MAGIC_NUMBER '?'

#define IOCTL_EXAMPLE_ADDITION _IO(IOCTL_EXAMPLE_MAGIC_NUMBER, 0)

#define IOCTL_EXAMPLE_SET _IOW(IOCTL_EXAMPLE_MAGIC_NUMBER, 1, unsigned long)

#define IOCTL_EXAMPLE_GET _IOR(IOCTL_EXAMPLE_MAGIC_NUMBER, 2, unsigned long)

#define IOCTL_EXAMPLE_ADD _IOWR(IOCTL_EXAMPLE_MAGIC_NUMBER, 3, unsigned long)
```

And the following functionality for them:

```c
// ioctl_example.c
switch(command) {
        case IOCTL_EXAMPLE_SET: {
            return_value = get_user(tmp, (unsigned long __user*)argument);
            n_A = float_n_construct(tmp);
            addition = 0;
            return return_value;
        }
        case IOCTL_EXAMPLE_GET: {
            tmp = float_n_destruct(&n_A);
            return put_user(tmp, (unsigned long __user*)argument);
        }
        case IOCTL_EXAMPLE_ADD: {
            return_value = get_user(tmp, (unsigned long __user*)argument);
            n_B = float_n_construct(tmp);
            // This is just an example no carage or overflow is handled!
            n_B.integral += n_A.integral;
            n_B.fraction += n_A.fraction;
            addition = 1;
            tmp = float_n_destruct(&n_B);
            return put_user(tmp, (unsigned long __user*)argument) | return_value;
        }
        case IOCTL_EXAMPLE_ADDITION: {
            return addition;
        }
    }
    return 0;
```
And then exporting them as TypeScript const enum for in TypeScript use (exporting to JavaScript Objects is ofcorse available):

```c++
// ioctl_export.cc
#define IOCTL_ENUM_TS
#include "node_modules/ioctl-enum/ioctl-enum.h"
#include "ioctl_example.h"

IOCTL_ENUM("IOCTL_EXAMPLE");
IOCTL_ENUM_IOCTL("IOCTL_EXAMPLE_ADDITION", IOCTL_EXAMPLE_ADDITION);
IOCTL_ENUM_IOCTL("IOCTL_EXAMPLE_SET", IOCTL_EXAMPLE_SET);
IOCTL_ENUM_IOCTL("IOCTL_EXAMPLE_GET", IOCTL_EXAMPLE_GET);
IOCTL_ENUM_IOCTL("IOCTL_EXAMPLE_ADD", IOCTL_EXAMPLE_ADD);
IOCTL_ENUM_EXPORT();
```

And now controlling from TypeScript (well documented showing what is pass by value and pass by reference return from ioctl):

```typescript
// index.ts
import { ioctl, Ioctl } from 'ioctl-ulong';
import { IOCTL_EXAMPLE } from './IOCTL_EXAMPLE';
import { openIoctlSync } from 'open-ioctl';
import { closeSync } from 'fs';

function printIoctl(res: Ioctl) {
    console.log(`Ioctl: .ioctl = ${res.ioctl} .data = ${res.data}`);
}

// floating: float to be converted to unsigned long
function fromFloat(floating: number): number {
    let num: string[] = floating.toString().split('.'); // Forming [a, b]
    let n: number = Number(num[0]) << 16; // n = a{16 bits}0{16 bits}
    return n | Number(num[1]); // return a{16 bits}b{16 bits}
}

// ulong: unsigned long
function fromUlong(ulong: number): number {
    let integral: number = ulong >> 16; // integral = 16 Most significant bits from ulong
    let fraction: number = ulong & 65535; // integral = 16 Least significant bits from ulong
    return parseFloat(integral + '.' + fraction); // return Float(integral.fraction)
}

function checkForAddition(fd: number) {
    let additon: Ioctl = ioctl(fd, IOCTL_EXAMPLE.IOCTL_EXAMPLE_ADDITION); // _IO
    // .ioctl will be what ioctl returned if it was a non negative integer (value by value)
    // .data will be 0

    printIoctl(additon);
    console.log(`Addition: ${Boolean(additon.ioctl)}`); // Addition true | false
}

let result: Ioctl;
let fd: number = openIoctlSync('ioctl_example_dev'); // Open device driver in (3) ioctl non-blocking model;

result = ioctl(fd, IOCTL_EXAMPLE.IOCTL_EXAMPLE_SET, fromFloat(12.3451)); // _IOR
// .ioctl will be what ioctl returned if it was a non negative integer
// .data will be what was passed to ioctl

printIoctl(result);
console.log(`Setting: ${fromUlong(result.data)}`); // Setting: 12.3451

result = ioctl(fd, IOCTL_EXAMPLE.IOCTL_EXAMPLE_GET); // _IOW
// .ioctl will be what ioctl returned if it was a non negative integer
// .data will be what was passed from the kernel (device driver) (value by reference)

printIoctl(result);
console.log(`Getting: ${fromUlong(result.data)}`); // Getting: 12.3451

checkForAddition(fd); // Addition: false

result = ioctl(fd, IOCTL_EXAMPLE.IOCTL_EXAMPLE_ADD, fromFloat(23.5786)); // _IOWR
// .ioctl will be what ioctl returned if it was a non negative integer
// .data will be what was passed from the kernel (device driver) (value by reference)

printIoctl(result);
console.log(`Result: ${fromUlong(result.data)}`); // Result: 35.9237

checkForAddition(fd); // Addition: true

closeSync(fd); // Release device driver*/
```

## TypeScript

```typescript
// index.d.ts
interface Ioctl {
    ioctl: number;
    data: number;
}

declare function ioctl(fd: number, command: number, argument?: number): Ioctl;

export { Ioctl, ioctl };
```

# FAQ

# Q: Why implicit cast from uint32_t to unsinged long is needed ?

A: You can not be sure if the user mode uint32_t provided by compiler you are using matches the defention used by the kernel, by value they may match but when it comes to pointers (unless it uses in kernel definistion) they don't. Test if you don't belive me even gcc's definitions may vary from version to version, after all libc is just a user space program ...
