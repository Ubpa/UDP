function(Ubpa_AutoRefl input output)
  add_custom_command(
    OUTPUT ${output}
	COMMAND Ubpa::UDP_app_AutoRefl ARGS ${input} ${output}
	DEPENDS Ubpa::UDP_app_AutoRefl
  )
endfunction()
