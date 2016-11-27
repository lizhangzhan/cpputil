LIBNAME = libcpputil
LIBVER = 0.0.1
LIBVER_MAJOR = $(shell echo $(LIBVER) | cut -d. -f1)

#CXX_HOME = /usr/local/Cellar/gcc49/4.9.3/
#CXX := $(CXX_HOME)/bin/g++-4.9
CXX_HOME = /usr
CXX = /usr/bin/g++-4.4.7

LD_LIBRARY_PATH = $(CXX_HOME)/lib:$(CXX_HOME)/lib64:$(DEFAULT_LIB_INSTALL_PATH)
CXXFLAGS := \
	-g3 \
	-O2 \
	-fPIC \
	-Wall \
	-fno-strict-aliasing \

LDFLAGS := \
	-L $(CXX_HOME)/lib \
	-L $(CXX_HOME)/lib64

LIBS := \
	-lpthread

SRC := $(wildcard *.cc)
OBJ := $(patsubst *.cc, *.o, $(SRC))

SHARE_LIB = $(LIBNAME).so

all: shared

shared: $(SHARE_LIB).$(LIBVER) $(SHARE_LIB).$(LIBVER_MAJOR) $(SHARE_LIB)

$(SHARE_LIB).$(LIBVER) : $(OBJ)
	$(CXX) $^ -o $@  $(CXXFLAGS) $(LDFLAGS) $(LIBS) -shared -Wl,-soname,$(SHARE_LIB).$(LIBVER_MAJOR)

$(SHARE_LIB).$(LIBVER_MAJOR) : $(SHARE_LIB).$(LIBVER)
	ln -s $^ $@

$(SHARE_LIB) : $(SHARE_LIB).$(LIBVER)
	ln -s $^ $@

clean:
	rm -fr $(OBJ)
	rm  $(SHARE_LIB).$(LIBVER) $(SHARE_LIB).$(LIBVER_MAJOR) $(SHARE_LIB)

PACKAGE_DIRS = lib include
PACKAGE_NAME = $(LIBNAME)-$(LIBVER)
HEADER_NAMES := $(wildcard *.h)

install: shared
	rm -fr packages/$(PACKAGE_NAME)
	mkdir -p packages/$(PACKAGE_NAME)
	@for d in $(PACKAGE_DIRS); do \
		if test ! -d packages/$(PACKAGE_NAME)/$$d; then \
			mkdir -p packages/$(PACKAGE_NAME)/$$d; \
		fi \
	done
	/usr/bin/install -c   $(SHARE_LIB).$(LIBVER) packages/$(PACKAGE_NAME)/lib
	/usr/bin/install -c   $(HEADER_NAMES) packages/$(PACKAGE_NAME)/include
	/sbin/ldconfig -n packages/$(PACKAGE_NAME)/lib
	(cd packages/$(PACKAGE_NAME)/lib; ln -s $(SHARED_LIB).$(LIBVER_MAJOR) $(SHARED_LIB))

.PHONY: all clean
