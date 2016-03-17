
env = Environment(tools=['mingw'],
    CPPPATH=['src'],
    CFLAGS='-Wall --std=c99'
    )

source = [
    'src/app/main.c',
    'src/vm/slip_vm.c',
]

env.Program(target='build/slip', source=source)