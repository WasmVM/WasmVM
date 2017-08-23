include_directories(.. ../Instance ../Component ../Exception ../Instruction)
add_library( WasmVM_Syscall SHARED
	Syscall.cpp
	Call40.cpp
	Call20.cpp
)