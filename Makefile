#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.156                       #
#------------------------------------------------------------------------------#


WORKDIR = $PWD

CC = /usr/bin/i686-linux-gnu-gcc -m32
CXX = /usr/bin/i686-linux-gnu-g++ -m32
AR = /usr/bin/i686-linux-gnu-gcc-ar
LD = $(CXX)
WINDRES =

INC = -I/usr/include/SDL2
CFLAGS = -Wall -MMD
RCFLAGS = 
RESINC = 
LIBDIR = -L/usr/lib/i386-linux-gnu
LIB = -lSDL2 -lSDL2_mixer
LDFLAGS =
DEPEND = makedepend

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -g
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS)
OBJDIR_RELEASE = ./obj/Ubuntu
OUT_RELEASE = ./Blake4SDL_Ubuntu

SOURCES = id_ca.c id_in.c id_pm.c id_sd.c id_us.c id_vw.c colormap.c \
	jm_io.c jm_lzh.c jm_tp.c mame/fmopl.c movie.c platutil.c \
	3d_act2.c 3d_agent.c 3d_debug.c 3d_draw.c 3d_game.c 3d_inter.c \
	3d_main.c 3d_menu.c 3d_act1.c 3d_msgs.c 3d_plane.c 3d_play.c \
	3d_scale.c 3d_state.c 3d_utils.c

OBJ_RELEASE = $(patsubst %.c,$(OBJDIR_RELEASE)/%.o,$(SOURCES))

all: release

clean: clean_release

before_release: 
	test -d . || mkdir -p .
	test -d $(OBJDIR_RELEASE)/mame || mkdir -p $(OBJDIR_RELEASE)/mame

release: before_release out_release

out_release: before_release $(OBJ_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/%.d: %.c
	$(DEPEND) -f- -p$(OBJDIR_RELEASE)/ $< > $@

$(OBJDIR_RELEASE)/%.o: %.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c $< -o $@

-include $(wildcard $(OBJDIR_RELEASE)/*.d $(OBJDIR_RELEASE)/mame/*.d)

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf $(OBJDIR_RELEASE)

.PHONY: before_release after_release clean_release

