.PHONY: all clean

LIBS = ./My_functions/ ./Expression_tree/ ./Name_table/

all:
	@for lib in $(LIBS); do	\
		$(MAKE) -C $$lib;	\
	done

clean:
	@for lib in $(LIBS); do		\
		$(MAKE) clean -C $$lib;	\
	done
