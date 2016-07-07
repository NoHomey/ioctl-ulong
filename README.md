# ioctl-ulong

unux'es ioctl addon wrapper working with unsigned long (both at user and kernel level, uses implicit cast from uit32_t to unsigned long so the limit is 4294967295 or 2^32)

# Installation

Install with npm:

```bash
$ npm install ioctl-ulong
```

# Purpose

ioctl-ulong it's not just another ioctl C++ addon wrapper for Node.js. It's not even fully campatible ioctl C++ addon wrapper. It's only purpose is to transfer largest posible unsigned integers from Node.js to to kernel (for now this are uint32_t) and to do it fast as possible. It dosen't provide the functionality to transfer struct like data like ioctl wrapper found at npm (https://www.npmjs.com/package/ioctl). So if you want to transfer more data at once use ioctl instead or do bit shifting operations like in the example. Bit shifting operations are required some times like intensive use of integer and floats in kernel module where switching context from integer to floating mode cpu do costs performence. Because of uint32_t beeing the limit the largest possible number wich will correctly be transfered to your driver is 4294967295 or 2^32.

Also ioctl-ulong is TypeScript ready, it ships with definition file.

## To install ioctl via npm do:

```basb
npm install ioctl
```

# Usage

## ioctl(fd, ioctl_number, argumet?)

### fd: opened file descriptor (int32_t)

### ioctl_number: ioctl number to be trasfered to your driver (uint32_t)

### argument: optional number to be transfered to your driver as third argument (uint32_t)

# Example

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
