CC= gcc
CFLAGS=  -Wall -g
iconDir = icon
headersDir = header
objectsDir = object
iconFormat = .o
headerFormat = .h
objectFormat = .o
staticSufix = _static_
dynamicSufix = _dynamic_
dLibMakeFormat = .d
sLibMakeFormat = .s
includesFormat = .i
sLibFormat = .a
dLibFormat = .dll
codeFormat = .c
binaryFormat = .exe
binaryMakeFormat = .b
headers = $(addprefix -include ,$(wildcard $(headersDir)/*$(headerFormat)))
objects  = $(wildcard $(objectsDir)/*$(objectFormat))
staticLibrarys := $(wildcard *$(sLibFormat))
dynamicLibrarys = $(wildcard *$(dLibFormat))
getAllIncludes = $(shell CMD //C @echo off \& for //F "tokens=2-3 delims= " %E in \(\'FINDSTR //b //r //c:"\#include *" $1\'\) do echo %E %F)
getIncludes = $(shell echo $(subst $(headerFormat),,$(subst $(subst $(includesFormat),,$2)$(headerFormat),,$(subst $()  $(),$() $(),$(patsubst <%>,,$1)))))
getLibIncludes = $(foreach var,$1,$(shell [ -f $(var)$2 ] && echo $(var)$2 || echo $(var)$3))
getUpperCase = $(shell echo '$1' | tr '[:lower:]' '[:upper:]')
getStaticDefinitions = $(call getUpperCase, $(subst $(sLibFormat),$(staticSufix),$(addprefix -D ,$1)))
prefferDynamic=1
appName = tester

# Clean all objects.
cleanObjs:
	@if [ ! -d "$(objectsDir)" ]; then mkdir -p $(objectsDir); fi;
	@for obj in $(objects); do rm -f $$obj ; done

# Clean all static lib with source file.
cleanAllSLib:
	$(eval libs := $(subst $(sLibFormat),,$(staticLibrarys)))
	@for lib in $(libs); do\
		if [ -f "$$lib$(codeFormat)" ]; then \
			rm -f $$lib$(sLibFormat); \
		fi;\
	done

# Clean all dynamic lib with source file.
cleanAllDLib:
	$(eval libs := $(subst $(dLibFormat),,$(dynamicLibrarys)))
	@for lib in $(libs); do\
		if [ -f "$$lib$(codeFormat)" ]; then \
			rm -f $$lib$(dLibFormat); \
		fi;\
	done

# Clean all.
clean:
	make cleanObjs
	make cleanAllSLib
	make cleanAllDLib
	@if [ -f "$(appName)$(binaryFormat)" ]; then rm -f $(appName)$(binaryFormat); fi;

# Make object static.
%$(objectFormat)$(sLibMakeFormat): %$(codeFormat)
	$(eval name := $(patsubst %$(objectFormat)$(sLibMakeFormat),%,$@))
	$(CC) $(CFLAGS) -I $(headersDir) -c -o $(objectsDir)/$(name)$(objectFormat) $< -D BUILDING -D $(call getUpperCase,$(name))_DEFINITIONS_ -D STATIC_LIB_

# Make object dynamic.
%$(objectFormat)$(dLibMakeFormat): %$(codeFormat)
	$(eval name := $(patsubst %$(objectFormat)$(dLibMakeFormat),%,$@))
	$(CC) $(CFLAGS) -I $(headersDir) -c -o $(objectsDir)/$(name)$(objectFormat) $< -D BUILDING -D $(call getUpperCase,$(name))_DEFINITIONS_

# Make object dynamic without DllMain.
%$(objectFormat)$(dLibMakeFormat)DllMain: %$(codeFormat)
	$(eval name := $(patsubst %$(objectFormat)$(dLibMakeFormat)DllMain,%,$@))
	$(CC) $(CFLAGS) -I $(headersDir) -c -o $(objectsDir)/$(name)$(objectFormat) $< -D BUILDING -D $(call getUpperCase,$(name))_DEFINITIONS_ -D NO_DLL_MAIN_

# Make includes.
%$(includesFormat): %$(codeFormat)
	$(eval includes := $(call getIncludes,$(call getAllIncludes, $<),$@))
	$(eval firstIncludes := $(shell [ $(prefferDynamic) = 0 ] && echo $(sLibFormat) || echo $(dLibFormat)))
	$(eval secondIncludes := $(shell [ $(firstIncludes) = $(dLibFormat) ] && echo $(sLibFormat) || echo $(dLibFormat)))
	$(eval includes := $(call getLibIncludes,$(includes),$(firstIncludes),$(secondIncludes)))
	@echo "$(includes)"

# Make static lib.
%$(sLibMakeFormat): %$(codeFormat)
	$(eval name := $(patsubst %$(sLibMakeFormat),%,$@))
	make $(name)$(objectFormat)$(sLibMakeFormat)
	$(eval includes := $(call getIncludes,$(call getAllIncludes, $(name)$(codeFormat)),$(name)$(includesFormat)))
	$(eval firstIncludes := $(shell [ $(prefferDynamic) = 0 ] && echo $(sLibFormat) || echo $(dLibFormat)))
	$(eval secondIncludes := $(shell [ $(firstIncludes) = $(dLibFormat) ] && echo $(sLibFormat) || echo $(dLibFormat)))
	$(eval includes := $(call getLibIncludes,$(includes),$(firstIncludes),$(secondIncludes)))
	@echo "Includes: $(includes)"
	@if [ -f "$(name)$(sLibFormat)" ]; then rm -f $(name)$(sLibFormat); fi;
	ar -rcsv -o $(name)$(sLibFormat) $(objectsDir)/$(name)$(objectFormat) $(includes)

# Make dynamic lib
%$(dLibMakeFormat): %$(codeFormat)
	$(eval name := $(patsubst %$(dLibMakeFormat),%,$@))
	make $(name)$(objectFormat)$(dLibMakeFormat)
	$(eval includes := $(call getIncludes,$(call getAllIncludes, $(name)$(codeFormat)),$(name)$(includesFormat)))
	$(eval firstIncludes := $(shell [ $(prefferDynamic) = 0 ] && echo $(sLibFormat) || echo $(dLibFormat)))
	$(eval secondIncludes := $(shell [ $(firstIncludes) = $(dLibFormat) ] && echo $(sLibFormat) || echo $(dLibFormat)))
	$(eval includes := $(call getLibIncludes,$(includes),$(firstIncludes),$(secondIncludes)))
	@echo "Includes: $(includes)"
	@if [ -f "$(name)$(dLibFormat)" ]; then rm -f $(name)$(dLibFormat); fi;
	$(CC) $(CFLAGS) -shared -o $(name)$(dLibFormat) $(objectsDir)/$(name)$(objectFormat) $(includes) -Wl,--add-stdcall-alias

# Make c-file withouth libs.
%$(binaryMakeFormat): %$(codeFormat)
	$(eval name := $(patsubst %$(binaryMakeFormat),%,$@))
	$(CC) $(CFLAGS) -I $(headersDir) -o $(name)$(binaryFormat) $(name)$(codeFormat) $(iconDir)/$(name)$(iconFormat)

# Make c-file withouth libs and run.
%$(binaryMakeFormat)R: %$(codeFormat)
	$(eval name := $(patsubst %$(binaryMakeFormat)R,%,$@))
	$(CC) $(CFLAGS) -I $(headersDir) -o $(name)$(binaryFormat) $(name)$(codeFormat) $(iconDir)/$(name)$(iconFormat)
	./$(name)$(binaryFormat)

# Make The application without libs.
appN: $(appName).c
	make $(appName)$(binaryMakeFormat)

# Make The application without libs and run.
appNR:
	make $(appName)$(binaryMakeFormat)R

# Make The application
app: $(appName)$(codeFormat)
	$(eval includes := $(call getIncludes,$(call getAllIncludes, $(appName)$(codeFormat)),$(appName)$(includesFormat)))
	$(eval firstIncludes := $(shell [ $(prefferDynamic) = 0 ] && echo $(sLibFormat) || echo $(dLibFormat)))
	$(eval secondIncludes := $(shell [ $(firstIncludes) = $(dLibFormat) ] && echo $(sLibFormat) || echo $(dLibFormat)))
	$(eval includes := $(call getLibIncludes,$(includes),$(firstIncludes),$(secondIncludes)))
	$(eval static := $(patsubst %$(dLibFormat),,$(includes)))
	@echo Includes: $(includes)
	@echo Static:   $(static)
	$(CC) $(CFLAGS) -I $(headersDir) -o $(appName) $(appName)$(codeFormat) $(includes) $(iconDir)/$(appName)$(iconFormat) $(call getStaticDefinitions, $(static)) -D $(call getUpperCase,$(appName))_DEFINITIONS_

# Make The application an run.
appR:
	make app
	./$(appName)

# Make all the targets.
all:
	make clean
	make dLib
	make app
	./$(appName)

.PHONY: cleanObjs cleanAllSLib cleanAllDLib clean sLib dLib app appN appR appNR all
