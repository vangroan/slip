
import os

env = Environment(tools=['mingw'],
    CPPPATH=['src'],
    CFLAGS='-g -Wall --std=c99',
    # TODO: Find better way to point to compiler
    ENV={ 'PATH' : os.environ.get('PATH') },
    )

source = [
    'src/vm/slip_vm.c',
    'src/util/slip_io.c',
]

app_source = source + ['src/app/main.c',]

env.Program(target='build/slip', source=app_source)
env.StaticLibrary(target='build/lib/slip', source=source)

# Tests
slip_lib = File('build/lib/libslip.a')
test_source = [
    'src/test/main.c'
]
env.Program(target='build/test', source=test_source, LIBS=[slip_lib])

