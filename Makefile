CC := gcc
CFLAGS := -O3 -D__USE_INLINE__
LIBS :=

OBJS := guideml.o
DEPS := GuideML_rev.h

all: GuideML_OS4

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

GuideML_OS4: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
