dnl ---------------------------------------------------------------------------
dnl This file offers the following macros rlated to curses and x11
dnl ---------------------------------------------------------------------------
dnl MH_CHECK_X_INC
dnl MH_CHECK_X_LIB
dnl MH_CHECK_X_HEADERS
dnl MH_CHECK_X_KEYDEFS
dnl MH_CHECK_X_TYPEDEF
dnl MH_CHECK_CURSES

dnl ---------------------------------------------------------------------------
dnl Determine if the supplied X headers exist.
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_X_HEADERS],
[
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $MH_XINC_DIR"
for mh_header in $1; do
   AC_CHECK_HEADERS($mh_header)
done
CPPFLAGS="$save_CPPFLAGS"
])dnl

dnl ---------------------------------------------------------------------------
dnl Determine if various key definitions exist in keysym.h
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_X_KEYDEFS],
[
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $MH_XINC_DIR"
for mh_keydef in $1; do
   AC_MSG_CHECKING(for $mh_keydef in keysym.h)
   mh_upper_name="HAVE_`echo $mh_keydef | tr '[a-z]' '[A-Z]'`"
   AC_TRY_COMPILE([#include <keysym.h>],
[int i = $mh_keydef;],
  mh_have_key=yes; AC_DEFINE_UNQUOTED($mh_upper_name,1), mh_have_key=no )
   AC_MSG_RESULT($mh_have_key)
done
CPPFLAGS="$save_CPPFLAGS"
])dnl

dnl ---------------------------------------------------------------------------
dnl Determine if supplied types have been typedefed
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_X_TYPEDEF],
[
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $SYS_DEFS $MH_XINC_DIR"
for td in $1 ; do
AC_MSG_CHECKING(if $td is typedefed:)
AC_TRY_COMPILE(
[#include "X11/Xlib.h"],
[$td fred],
   [mh_td=yes],
   [mh_td=no]
)
if test "$mh_td" = yes ; then
   TD_upper=`echo $td | tr a-z A-Z`
   AC_DEFINE_UNQUOTED(${TD_upper}_TYPEDEFED, 1)
fi
AC_MSG_RESULT($mh_td)
done
CPPFLAGS="$save_CPPFLAGS"
])dnl

dnl ---------------------------------------------------------------------------
dnl Set up the correct X header file location
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_X_INC],
[
AC_MSG_CHECKING(for location of X headers)
MH_FOUND_INC_DIR=no
dnl if building on macos do not use pkg-config
if test "$on_macos" != yes; then
   if test "$with_xaw3d" = yes; then
      XAW_PKG="xaw3d xpm sm ice xext xmu"
   else
      XAW_PKG="xaw7 xpm sm ice xext xmu"
   fi
   pkg-config --exists ${XAW_PKG}
   AC_CHECK_PROG(pkg_config, [pkg-config], "yes", no)
   if test "$pkg_config" = yes; then
      # we have pkg_config, do we have xaw?
      pkg-config --exists ${XAW_PKG} > /dev/null
      if test $? -eq 0; then
         MH_XINC_DIR="`pkg-config --cflags ${XAW_PKG}`"
         AC_MSG_RESULT(found via pkg_config $MH_XINC_DIR)
         AC_SUBST(MH_XINC_DIR)
         MH_FOUND_INC_DIR=yes
      fi
   fi
fi
if test "$MH_FOUND_INC_DIR" = no; then
   MH_CHECK_X_INC_MANUAL()
fi
])dnl

AC_DEFUN([MH_CHECK_X_INC_MANUAL],
[
mh_x11_dir=""
mh_x11_xaw_dir=""
dnl
dnl specify latest release of X directories first
dnl
mh_inc_dirs="\
#    $HOME/include/X11         \
    $HOME/include             \
#    /tmp/include/X11          \
    /usr/include              \
    /usr/local/include        \
    /tmp/include              \
    /usr/X11R6/include        \
    /usr/include/X11R6        \
    /usr/local/X11R6/include  \
    /usr/local/include/X11R6  \
    /usr/X11R5/include        \
    /usr/include/X11R5        \
    /usr/local/X11R5/include  \
    /usr/local/include/X11R5  \
    /usr/local/x11r5/include  \
    /usr/X11R4/include        \
    /usr/include/X11R4        \
    /usr/local/X11R4/include  \
    /usr/local/include/X11R4  \
                              \
#    /usr/X11/include          \
#    /usr/include/X11          \
#    /usr/local/X11/include    \
#    /usr/local/include/X11    \
                              \
    /usr/X386/include         \
    /usr/x386/include         \
    /usr/XFree86/include/X11  \
                              \
    /usr/unsupported/include  \
    /usr/athena/include       \
    /usr/lpp/Xamples/include  \
                              \
    /usr/openwin/include      \
    /usr/openwin/share/include"
dnl
dnl Provide for user supplying directory
dnl
if test "x$x_includes" != xNONE ; then
   mh_inc_dirs="$x_includes $mh_inc_dirs"
fi

dnl
dnl Try to determine the directory containing X headers
dnl We will append X11 to all the paths above as an extra check
dnl
for ac_dir in $mh_inc_dirs ; do

dnl  if test -r $ac_dir/Intrinsic.h; then
dnl    mh_x11_dir=$ac_dir
dnl    break
dnl  fi
  if test -r $ac_dir/X11/Intrinsic.h; then
    mh_x11_dir="$ac_dir"
    break
  fi
done

dnl
dnl Try to determine the directory containing Xaw headers
dnl We will append X11 to all the paths above as an extra check
dnl
if test "$with_xaw3d" = yes; then
   mh_xaw_dir="Xaw3d"
else
   if test "$with_nextaw" = yes; then
      mh_xaw_dir="neXtaw"
   else
      mh_xaw_dir="Xaw"
   fi
fi
for ac_dir in $mh_inc_dirs ; do
dnl  if test -r $ac_dir/$mh_xaw_dir/Box.h; then
dnl    mh_x11_xaw_dir=$ac_dir
dnl    break
dnl  fi
  if test -r $ac_dir/X11/$mh_xaw_dir/Box.h; then
    mh_x11_xaw_dir="$ac_dir"
    break
  fi
done

if test "x$mh_x11_dir" != "x" ; then
   mh_x11_dir_no_x11=`echo $mh_x11_dir | sed 's/\/X11$//'`
dnl
dnl Test to see if $mh_x11_dir_no_x11 is /usr/include and we are using gcc
dnl under Solaris. If so, ignore it.
   AC_REQUIRE([AC_CANONICAL_SYSTEM])
   mh_solaris_gcc_usr_include="no"
   case "$target" in
      *solaris*)
         if test "$ac_cv_prog_CC" = "gcc" -a "$mh_x11_dir_no_x11" = "/usr/include" ; then
            mh_solaris_gcc_usr_include="yes"
         fi
         ;;
   esac
   if test "$mh_x11_dir_no_x11" != "$mh_x11_dir" -a "$mh_solaris_gcc_usr_include" = "no" ; then
      MH_XINC_DIR="-I$mh_x11_dir -I$mh_x11_dir_no_x11"
   else
      MH_XINC_DIR="-I$mh_x11_dir"
   fi
else
   AC_MSG_ERROR(Cannot find required header file Intrinsic.h; XCurses cannot be configured)
fi

if test "x$mh_x11_xaw_dir" != "x" ; then
   MH_XINC_DIR="-I$mh_x11_xaw_dir $MH_XINC_DIR"
else
   AC_MSG_ERROR(Cannot find required Xaw header file Box.h; XCurses cannot be configured)
fi

   AC_MSG_RESULT(found in $mh_x11_dir $mh_x11_xaw_dir)
   AC_SUBST(MH_XINC_DIR)

])dnl

dnl ---------------------------------------------------------------------------
dnl Set up the correct X library file location
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_X_LIB],
[
MH_FOUND_LIB_DIR=no
dnl if building on macos do not use pkg-config
if test "$on_macos" != yes; then
   if test "$with_xaw3d" = yes; then
      XAW_PKG="xaw3d xpm sm ice xext xmu"
   else
      XAW_PKG="xaw7 xpm sm ice xext xmu"
   fi
   AC_CHECK_PROG(pkg_config, [pkg-config], "yes", no)
   if test "$pkg_config" = yes; then
      # we have pkg_config, do we have xaw?
      pkg-config --exists ${XAW_PKG} > /dev/null
      if test $? -eq 0; then
         MH_XLIBS="`pkg-config --libs ${XAW_PKG}`"
         AC_MSG_RESULT(found via pkg_config $MH_XLIBS)
         AC_SUBST(MH_XLIBS)
         MH_FOUND_LIB_DIR=yes
      fi
   fi
fi
if test "$MH_FOUND_LIB_DIR" = no; then
   MH_CHECK_X_LIB_MANUAL()
fi
])dnl

AC_DEFUN([MH_CHECK_X_LIB_MANUAL],
[
dnl
dnl Some systems require extra libraries...
dnl
mh_solaris_flag=no
mh_hpux9_flag=no
AC_REQUIRE([AC_CANONICAL_SYSTEM])
case "$target" in
   *solaris*)
      mh_solaris_flag=yes
      ;;
   *pc-sco*)
      extra_x_libs="Xext"
      ;;
   sparc*sunos*)
      extra_x_libs="Xext"
      if test "$ac_cv_prog_CC" = "gcc" ; then
         extra_ld_flags="-Wl,-Bstatic"
         extra_ld_flags2="-Wl,-Bdynamic"
      else
         extra_ld_flags="-Bstatic"
         extra_ld_flags2="-Bdynamic"
      fi
      ;;
   *hpux9*)
      mh_hpux9_flag=yes
      ;;
esac

AC_MSG_CHECKING(for location of X libraries)
if test "$with_xaw3d" = yes; then
   MH_X11_LIBS="Xaw3d Xmu Xt X11"
else
   if test "$with_nextaw" = yes; then
      MH_X11_LIBS="neXtaw Xmu Xt X11"
   else
      MH_X11_LIBS="Xaw Xmu Xt X11"
   fi
fi
MH_X11R6_LIBS="Xext Xpm"
mh_x11r6=no

which dpkg-architecture > /dev/null
if test $? -eq 0; then
   multiarch_libdir="/usr/lib/`dpkg-architecture -qDEB_HOST_MULTIARCH`"
else
   multiarch_libdir=""
fi
dnl
dnl specify latest release of X directories first
dnl
mh_lib_dirs="$multiarch_libdir \
    $HOME/lib64           \
    $HOME/lib32           \
    $HOME/lib             \
    /tmp/lib              \
    /usr/X11R6/lib64      \
    /usr/X11R6/lib32      \
    /usr/X11R6/lib        \
    /usr/lib64/X11R6      \
    /usr/lib32/X11R6      \
    /usr/lib/X11R6        \
    /usr/local/X11R6/lib64  \
    /usr/local/X11R6/lib32  \
    /usr/local/X11R6/lib  \
    /usr/local/lib64/X11R6  \
    /usr/local/lib32/X11R6  \
    /usr/local/lib/X11R6  \

    /usr/X11R5/lib        \
    /usr/lib/X11R5        \
    /usr/local/X11R5/lib  \
    /usr/local/lib/X11R5  \
    /usr/local/x11r5/lib  \

    /usr/X11R4/lib        \
    /usr/lib/X11R4        \
    /usr/local/X11R4/lib  \
    /usr/local/lib/X11R4  \
                          \
    /usr/X11/lib          \
    /usr/lib/X11          \
    /usr/local/X11/lib    \
    /usr/local/lib/X11    \
                          \
    /usr/X386/lib         \
    /usr/x386/lib         \
    /usr/XFree86/lib/X11  \
                          \
    /usr/lib64            \
    /usr/lib32            \
    /usr/lib              \
    /usr/local/lib64      \
    /usr/local/lib32      \
    /usr/local/lib        \
    /usr/unsupported/lib  \
    /usr/athena/lib       \
    /usr/lpp/Xamples/lib  \
                          \
    /usr/openwin/lib      \
    /usr/openwin/share/lib"
dnl
dnl Provide for user supplying directory
dnl
if test "x$x_libraries" != xNONE ; then
   mh_lib_dirs="$x_libraries $mh_lib_dirs"
fi

dnl
dnl try to find libSM.[a,sl,so,dylib]. If we find it we are using X11R6
dnl
for ac_dir in $mh_lib_dirs ; do
   for mh_xsm in libSM.a libSM.so libSM.sl libSM.dylib; do
     if test -r $ac_dir/$mh_xsm; then
       mh_x11r6=yes
       break 2
     fi
   done
done

if test "$mh_x11r6" = yes ; then
   mh_libs="$MH_X11_LIBS $MH_X11R6_LIBS"
else
   mh_libs="$MH_X11_LIBS $extra_x_libs"
fi

dnl
dnl Ensure that all required X libraries are found
dnl
mh_prev_dir=""
mh_where_found=""
mh_where_found_dirs=""
mh_solaris_path=""
for mh_lib in $mh_libs; do
  mh_lib_found=no
  for ac_dir in $mh_lib_dirs ; do
    for mh_ext in a so sl dylib dll.a; do
      if test -r $ac_dir/lib$mh_lib.$mh_ext; then
        if test "x$mh_prev_dir" != "x$ac_dir" ; then
          if test "x$mh_prev_dir" = "x" ; then
             mh_where_found="$mh_where_found found in $ac_dir"
          else
             mh_where_found="$mh_where_found and in $ac_dir"
          fi
          mh_prev_dir=$ac_dir
          mh_where_found_dirs="$mh_where_found_dirs $ac_dir"
          MH_XLIBS="$MH_XLIBS -L$ac_dir"
          mh_solaris_path="${mh_solaris_path}:$ac_dir"
        fi
        MH_XLIBS="$MH_XLIBS -l$mh_lib"
        mh_lib_found=yes
        break 2
      fi
    done
  done
  if test "$mh_lib_found" = no; then
    AC_MSG_ERROR(Cannot find required X library; lib$mh_lib. XCurses cannot be configured)
  fi
done
AC_MSG_RESULT($mh_where_found)
mh_solaris_path=`echo $mh_solaris_path | sed 's/^://'`
if test "$mh_solaris_flag" = yes ; then
   MH_XLIBS="-R$mh_solaris_path $extra_ld_flags $MH_XLIBS $extra_libs $extra_ld_flags2"
else
   MH_XLIBS="$extra_ld_flags $MH_XLIBS $extra_libs $extra_ld_flags2"
fi
if test "$mh_hpux9_flag" = yes ; then
  grep -q XtSetLanguageProc $mh_x11_dir/Intrinsic.h
  if test $? -eq 0 ; then
    mh_found_xtshellstrings=no
    for mh_acdir in $mh_where_found_dirs ; do
      for mh_xaw in `ls $mh_acdir/libXaw.*` ; do
        nm $mh_xaw | grep XtShellStrings | grep -qv extern
        if test $? -eq 0 ; then
          mh_found_xtshellstrings=yes
        fi
      done
    done
    if test "$mh_found_xtshellstrings" = no ; then
      AC_MSG_WARN(The X11 development environment has not been installed correctly.)
      AC_MSG_WARN(The header file; Intrinsic.h, is for X11R5 while the Athena Widget)
      AC_MSG_WARN(Set library; libXaw is for X11R4.  This is a common problem with)
      AC_MSG_WARN(HP-UX 9.x.)
      AC_MSG_WARN(A set of required X11R5 library files can be obtained from the)
      AC_MSG_WARN(anonymous ftp sites listed on the PDCurses WWW home page:)
      AC_MSG_WARN(http://www.lightlink.com/hessling/)
      AC_MSG_WARN(The file is called HPUX-9.x-libXaw-libXmu.tar.Z)
      AC_MSG_ERROR(X11 installation incomplete; cannot continue)
    fi
  fi
fi
   AC_SUBST(MH_XLIBS)
])dnl

AC_DEFUN([MH_CHECK_PDCURSES_FONT],
[
PDCFONTPATH=""
AC_CHECK_PROG(fc_list, [fc-list], yes, no)
if test "$ac_cv_prog_fc_list" = yes; then
   PDC_FONT_PATH="`fc-list | grep DejaVuSansMono.ttf | head -1 | cut -d: -f1`"
   if test "x$PDC_FONT_PATH" = "x"; then
      AC_MSG_ERROR(--with-curses=$with_curses requires DejaVuSansMono.ttf font installed; THE cannot be configured)
   else
      AC_DEFINE_UNQUOTED(PDC_FONT_PATH, "$PDC_FONT_PATH", [default PDCursesMod font])
      AC_MSG_RESULT(default font is $PDC_FONT_PATH)
      PDCFONTPATH="-DPDC_FONT_PATH=\\\"$PDC_FONT_PATH\\\""
   fi
else
   AC_MSG_ERROR(--with-curses=$with_curses requires fc-list from fontconfig package installed; THE cannot be configured)
fi
AC_SUBST(PDCFONTPATH)
])

AC_DEFUN([MH_CHECK_NCURSES],
[
curs=$1
NCURSES_PKG="${curs}"
# check if --with-cursesincdir and --with-curseslibdir are specified
if test "$with_cursesincdir" = "no" -a "$with_curseslibdir" = "no"; then
   # look for pkg-config first
   AC_CHECK_PROG(pkg_config, [pkg-config], "yes", no)
   if test "$pkg_config" = yes; then
      # we have pkg_config, do we have ncurses[w]?
      pkg-config --exists ${NCURSES_PKG} > /dev/null
      if test $? -eq 0; then
         MH_CURSES_INC="`pkg-config --cflags ${NCURSES_PKG}`"
         MH_CURSES_LIB="`pkg-config --libs ${NCURSES_PKG}`"
         CURSES_BUILD=$with_curses
         CURSES_CONFIG="pkg-config ${NCURSES_PKG}"
         curses_h="curses.h"
         curses_l="$NCURSES_PKG"
         AC_DEFINE(USE_NCURSES)
      else
         # no pkg-config, what about ncurses-config?
         if test "$curs" = "ncursesw" -o "$curs" = "ncursesw8"; then
            curses_configs="ncursesw-config ncursesw6-config ncursesw5-config"
         else
            curses_configs="ncurses-config ncurses6-config ncurses5-config"
         fi
         for config_name in $curses_configs ;
            do
               AC_CHECK_PROG([config_var], [$config_name], [yes], no)
               if test "x$config_var" = "xyes"; then
                  MH_CURSES_INC="`${config_name} --cflags`"
                  MH_CURSES_LIB="`${config_name} --libs`"
                  CURSES_BUILD=$with_curses
                  CURSES_CONFIG="${config_name}"
                  curses_h="curses.h"
                  curses_l="$NCURSES_PKG"
                  AC_DEFINE(USE_NCURSES)
                  break
               fi
               # ensure variable is not cached
               unset ac_cv_prog_config_var
               unset config_var
            done
         if test "x$curses_h" = "x"; then
            AC_MSG_ERROR(unable to find configuration for ${NCURSES_PKG} package; cannot configure)
         fi
      fi
   fi
else
   CURSES_BUILD=$with_curses
   CURSES_CONFIG="user specified switches; --with-cursesincdir and --with-curseslibdir"
   curses_h="ncurses.h"
   curses_l="$NCURSES_PKG"
   MH_CURSES_INC="-I$with_cursesincdir"
   MH_CURSES_LIB="-L$with_curseslibdir -l$curses_l"
   AC_DEFINE(USE_NCURSES)
fi
])

AC_DEFUN([MH_CHECK_PDCURSES_X11],
[
curs=$1
builtin=$2
pkg=$3
flags=$4
libs=$5
AC_MSG_RESULT($1)
if test $builtin_curses = yes; then
   MH_CHECK_X_INC()
   MH_CHECK_X_LIB()
   MH_CURSES_INC="$flags -DXCURSES $MH_XINC_DIR"
   MH_CURSES_LIB="$libs $MH_XLIBS"
   CURSES_BUILD=$with_curses
   CURSES_CONFIG="builtin PDCursesMod"
   curses_h="curses.h"
   curses_l="pdcurses"
else
   if test "$with_cursesincdir" = no; then
      PDCURSES_PKG="lib${curs}"
      # look for pkg-config first
      AC_CHECK_PROG(pkg_config, [pkg-config], yes, no)
      if test "$ac_cv_prog_pkg_config" = yes; then
         # we have pkg_config, do we have ncursesw?
         pkg-config --exists ${PDCURSES_PKG} > /dev/null
         if test $? -eq 0; then
            MH_CURSES_INC="`pkg-config --cflags ${PDCURSES_PKG}`"
            MH_CURSES_LIB="`pkg-config --libs ${PDCURSES_PKG}`"
            curses_h="curses.h" #xcurses.h
            curses_l="XCurses"
         else
            AC_MSG_ERROR(pkg-config cannot find ${PDCURSES_PKG} package; cannot configure)
         fi
      fi
   else
      curses_h="curses.h"
      curses_l="XCurses"
      MH_CURSES_INC="$flags -I$with_cursesincdir"
      MH_CURSES_LIB="$with_curseslibdir/libXCurses.a $libs"
   fi
fi
])

AC_DEFUN([MH_CHECK_PDCURSES_SDL],
[
curs=$1
builtin=$2
pkg=$3
flags=$4
libs=$5
AC_MSG_RESULT($1)
if test $builtin_curses = yes; then
   # look for pkg-config first
   AC_CHECK_PROG(pkg_config, [pkg-config], yes, no)
   if test "$ac_cv_prog_pkg_config" = yes; then
      # we have pkg_config, do we have sdl or SDL_ttf?
      pkg-config --exists $pkg > /dev/null
      if test $? -eq 0; then
         MH_CURSES_INC="$flags `pkg-config --cflags $pkg`"
         MH_CURSES_LIB="$libs `pkg-config --libs $pkg`"
         CURSES_BUILD=$with_curses
         CURSES_CONFIG="builtin PDCursesMod"
         curses_h="curses.h"
         curses_l="pdcurses"
      else
         AC_MSG_ERROR(--with-curses=$with_curses requires $pkg package installed; THE cannot be configured)
      fi
   else
      AC_MSG_ERROR(--with-curses=$with_curses requires pkg-config and $pkg package installed; THE cannot be configured)
   fi
else
   if test "$with_cursesincdir" = no; then
      PDCURSES_PKG="lib${curs}"
      # look for pkg-config first
      AC_CHECK_PROG(pkg_config, [pkg-config], yes, no)
      if test "$ac_cv_prog_pkg_config" = yes; then
         # we have pkg_config, do we have pdcurses package?
         pkg-config --exists ${PDCURSES_PKG} > /dev/null
         if test $? -eq 0; then
            MH_CURSES_INC="`pkg-config --cflags ${PDCURSES_PKG}`"
            MH_CURSES_LIB="`pkg-config --libs ${PDCURSES_PKG}`"
            CURSES_BUILD=$with_curses
            CURSES_CONFIG="pkg-config ${PDCURSES_PKG}"
            curses_h="curses.h"
            curses_l="pdcurses"
         fi
      fi
   else
      curses_h="curses.h"
      curses_l="pdcurses"
      MH_CURSES_INC="$flags -I$with_cursesincdir"
      MH_CURSES_LIB="$with_curseslibdir/libpdcurses.a $libs"
   fi
fi

dnl --------------- allow --disable-font-check to disable test for DejaVu font
AC_ARG_ENABLE(font-check,
   [  --disable-font-check    disable check for DejaVuSansMono (used by SDL2W variant)],
   [enable_font_check=no],
   [enable_font_check=yes],
)
if test "$enable_font_check" = yes; then
   MH_CHECK_PDCURSES_FONT
fi
])

AC_DEFUN([MH_CHECK_PDCURSES_VT],
[
curs=$1
builtin_curses=$2
pkg=$3
flags=$4
libs=$5
AC_MSG_RESULT($1)
if test $builtin_curses = yes; then
   MH_CURSES_INC="$flags"
   MH_CURSES_LIB="$libs"
   CURSES_BUILD=$with_curses
   CURSES_CONFIG="builtin PDCursesMod"
   curses_h="curses.h"
   curses_l="pdcurses"
else
   if test "$with_cursesincdir" = no; then
      PDCURSES_PKG="lib${curs}"
      # look for pkg-config first
      AC_CHECK_PROG(pkg_config, [pkg-config], yes, no)
      if test "$ac_cv_prog_pkg_config" = yes; then
         # we have pkg_config, do we have ncursesw?
         pkg-config --exists ${PDCURSES_PKG} > /dev/null
         if test $? -eq 0; then
            MH_CURSES_INC="`pkg-config --cflags ${PDCURSES_PKG}`"
            MH_CURSES_LIB="`pkg-config --libs ${PDCURSES_PKG}`"
            CURSES_BUILD=$with_curses
            CURSES_CONFIG="pkg-config ${PDCURSES_PKG}"
            curses_h="curses.h"
            curses_l="pdcurses"
         fi
      fi
   else
      curses_h="curses.h"
      curses_l="pdcurses"
      MH_CURSES_INC="$flags -I$with_cursesincdir"
      MH_CURSES_LIB="$with_curseslibdir/libpdcurses.a $libs"
   fi
fi
])

dnl ---------------------------------------------------------------------------
dnl Check curses library and header files
dnl ---------------------------------------------------------------------------
AC_DEFUN([MH_CHECK_CURSES],
[
AC_MSG_CHECKING(curses platform)
builtin=$1
flags=$2
libs=$3
CURSES_BUILD=""
CURSES_CONFIG=""
MH_CURSES_INC=""
MH_CURSES_LIB=""
X11_LIBS="-lXaw -lXmu -lXt -lX11 -lXpm -lSM -lICE -lXext"
case "$target" in
   *hpux1*)
         mh_curses_colr=yes
         mh_pre_curses_h_include="stdarg"
      ;;
   *aix*4.2*)
         mh_curses_colr=no
         mh_pre_curses_h_include="stdarg"
      ;;
   *)
         mh_curses_colr=no
         mh_pre_curses_h_include=""
esac

# determine arguments for variant checks
case "$with_curses" in
   ncurses|ncursesw|ncursesw8)
      ;;
   pdcurses-x11)
      mcc_arg3=''
      mcc_arg4="$flags"
      mcc_arg5="$libs $X11_LIBS"
      ;;
   pdcurses-x11w)
      mcc_arg3=''
      mcc_arg4="$flags -DPDC_WIDE"
      mcc_arg5="$libs $X11_LIBS"
      ;;
   pdcurses-x11w8)
      mcc_arg3=''
      mcc_arg4="$flags -DPDC_WIDE -DPDC_FORCE_UTF8"
      mcc_arg5="$libs $X11_LIBS"
      ;;
   pdcurses-sdl1)
      mcc_arg3='sdl'
      mcc_arg4="$flags"
      mcc_arg5="$libs -lSDL"
      ;;
   pdcurses-sdl1w)
      mcc_arg3='SDL_ttf'
      mcc_arg4="$flags -DPDC_WIDE"
      mcc_arg5="$libs -lSDL -lSDL_ttf"
      ;;
   pdcurses-sdl1w8)
      mcc_arg3='SDL_ttf'
      mcc_arg4="$flags -DPDC_WIDE -DPDC_FORCE_UTF8"
      mcc_arg5="$libs -lSDL -lSDL_ttf"
      ;;
   pdcurses-sdl2)
      mcc_arg3='SDL2'
      mcc_arg4="$flags"
      mcc_arg5="$libs -lSDL2"
      ;;
   pdcurses-sdl2w)
      mcc_arg3='SDL2_ttf'
      mcc_arg4="$flags -DPDC_WIDE"
      mcc_arg5="$libs -lSDL2 -lSDL2_ttf"
      ;;
   pdcurses-sdl2w8)
      mcc_arg3='SDL2_ttf'
      mcc_arg4="$flags -DPDC_WIDE -DPDC_FORCE_UTF8"
      mcc_arg5="$libs -lSDL2 -lSDL2_ttf"
      ;;
   pdcurses-vt)
      mcc_arg3=''
      mcc_arg4="$flags"
      mcc_arg5="$libs"
      ;;
   pdcurses-vtw)
      mcc_arg3=''
      mcc_arg4="$flags -DPDC_WIDE"
      mcc_arg5="$libs"
      ;;
   pdcurses-vtw8)
      mcc_arg3=''
      mcc_arg4="$flags -DPDC_WIDE -DPDC_FORCE_UTF8"
      mcc_arg5="$libs"
      ;;
   *)
      # none specified
      ;;
esac

case "$with_curses" in
   ncurses|ncursesw|ncursesw8)
      MH_CHECK_NCURSES($with_curses)
      ;;
   pdcurses-x11*)
      MH_CHECK_PDCURSES_X11($with_curses,$builtin,$mcc_arg3,$mcc_arg4,$mcc_arg5)
      ;;
   pdcurses-sdl*)
      MH_CHECK_PDCURSES_SDL($with_curses,$builtin,$mcc_arg3,$mcc_arg4,$mcc_arg5)
      ;;
   pdcurses-vt*)
      MH_CHECK_PDCURSES_VT($with_curses,$builtin,$mcc_arg3,$mcc_arg4,$mcc_arg5)
      ;;
   *)
      # none specified
      ;;
esac

AC_SUBST(MH_CURSES_INC)
AC_SUBST(MH_CURSES_LIB)
CURSES_BUILD=$with_curses
AC_SUBST(CURSES_BUILD)
APK_CURSES_BUILD="`echo ${with_curses} | sed -e s/-/_/g`"
AC_SUBST(APK_CURSES_BUILD)
AC_MSG_RESULT($CURSES_BUILD)
])
