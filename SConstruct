
import os

vm_source = [
    'src/vm/slip_vm.c',
    'src/util/slip_io.c',
]

app_source = vm_source + ['src/app/main.c']

cli_env = Environment(tools=['mingw'],
    CPPPATH=['src', 'include'],
    CFLAGS='-g -Wall -Wextra -Wpedantic -m32 --std=c99',
	LIBS=['mingw32', 'SDL2main', 'SDL2'],
	LIBPATH='lib',
    # TODO: Find better way to point to compiler
    ENV={ 'PATH' : os.environ.get('PATH') },
    )

cli_env.Program(target='build/slip', source=app_source)


# TODO: Setup Scons properly to create two builds
# Tests
#slip_lib = File('build/lib/libslip.a')
#test_source = ['src/test/main.c']

#test_env = Environment(tools=['mingw'],
    #CPPPATH=['src', 'include'],
    #CFLAGS='-g -Wall -Wextra -Wpedantic -m32 --std=c99',
	#LIBS=[slip_lib],
    # TODO: Find better way to point to compiler
    #ENV={ 'PATH' : os.environ.get('PATH') },
    #)

#test_env.VariantDir('build', '.test/src', True)
#test_env.StaticLibrary(target='build/lib/slip', source=vm_source)
#test_env.Program(target='build/test', source=test_source)
