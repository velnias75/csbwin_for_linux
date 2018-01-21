ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src data

EXTRA_DIST = CSBwin-linux.sh.in

DISTCLEANFILES = CSBwin-linux.sh

bin_SCRIPTS = CSBwin-linux.sh

AM_DISTCHECK_CONFIGURE_FLAGS = --disable-esound --disable-dynwinsize

CSBwin-linux.sh: $(top_srcdir)/CSBwin-linux.sh.in
	$(AM_V_GEN)$(SED) \
	-e 's,@datadir[@],$(pkgdatadir),g' \
	-e 's,@prefix[@],$(prefix),g' \
	-e 's,@PACKAGE[@],$(PACKAGE),g' < $< > $@
