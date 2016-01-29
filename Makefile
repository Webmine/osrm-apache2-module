# Makefile for UNIX echo client and server

CXX=			g++ $(CCFLAGS)

OSRMd =	src/socket/unix-echo-server.o src/socket/unix-server.o server.o

OBJS =			$(OSRMd)

LIBS=

INCDIRS = include

CCFLAGS= -g

all:	src/socket/unix-echo-server src/socket/unix-echo-client

osrmd:$(OSRMd)
	$(CXX) -o server $(UNIX-ECHO-SERVER) $(LIBS)

clean:
	rm -f $(OBJS) $(OBJS:.o=.d)

realclean:
	rm -f $(OBJS) $(OBJS:.o=.d) server client


# These lines ensure that dependencies are handled automatically.
%.d:	%.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
		| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@'

include	$(OBJS:.o=.d)
