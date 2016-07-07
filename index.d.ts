interface Ioctl {
    ioctl: number;
    data: number;
}

declare function ioctl(fd: number, command: number, argument?: number): Ioctl;

export { Ioctl, ioctl };
