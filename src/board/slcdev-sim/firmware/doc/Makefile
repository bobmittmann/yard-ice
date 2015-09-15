include utils.mk

T2TFILES = $(wildcard *.t2t)

HTMLFILES = $(notdir $(T2TFILES:.t2t=.html))

all: $(HTMLFILES)

clean: 
	@$(RMALL) $(HTMLFILES)

.PHONY: all clean

