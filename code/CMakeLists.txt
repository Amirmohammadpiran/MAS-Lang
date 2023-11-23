add_executable (MAS-Lang
  main.cpp
  Lexer.cpp
  Parser.cpp
  Error.cpp
  )
target_link_libraries(MAS-Lang PRIVATE ${llvm_libs})
