{
    'targets': [
        {
            'target_name': 'ioctl_ulong',
            'sources': [ 'ioctl_ulong.cc' ],
            'include_dirs': [
                '<!(node -e "require(\'nan\')")'
            ]
        }
    ]
}
