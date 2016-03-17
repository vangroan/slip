
import os

env = Environment(tools=['mingw'],
    CPPPATH=['src'],
    CFLAGS='-g -Wall --std=c99',
    # TODO: Find better way to point to compiler
    ENV={ 'PATH' : os.environ.get('PATH') },
    )

source = [
    'src/app/main.c',
    'src/vm/slip_vm.c',
]

env.Program(target='build/slip', source=source)
