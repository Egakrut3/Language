.PHONY : test prepare

#TODO - very bad
test : prepare
	@./Front-end/bin/Test.exe --in ./Tree_in

#TODO - how to avoid -B option always
prepare :
	@make -B -C ./Front-end/
