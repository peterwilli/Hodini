{ stdenv, fetchzip, boost159, cmake, xorg, xcb-util-cursor, libxkbcommon, pkgconfig }:

stdenv.mkDerivation rec {
  pname = "hodini";
  version = "0";
  
  src = fetchzip {
    url = "https://github.com/peterwilli/Hodini/archive/12516f67724f85a45b7e7adaa7691301e718710b.zip";
    sha256 = "0jliawlsp2jpnp5b8iicypgrd0gx9fkcn6qsfc8yl65yyygcp40c";
  };

  sourceRoot = "source/driver";

  buildInputs = [ pkgconfig ];
  xorg-libs = [
    xorg.libXrender
    xorg.libxcb
    xorg.libXcomposite
    xorg.libXcursor
    xorg.libXext
    xorg.libXfixes
    xorg.libXi
    xorg.libXext
    xorg.libX11
    xorg.libX11.dev
    xorg.libX11.dev.out
    xorg.libXrandr
    xorg.libSM
    xorg.libICE
    xorg.libXinerama
    xorg.xcbproto
    xorg.xcbutil
    xorg.xcbutilwm
    xorg.xcbutilimage
    xorg.xcbutilerrors
    xorg.xcbutilkeysyms
    xorg.xkbutils
    xorg.xorgcffiles
    xcb-util-cursor
    libxkbcommon
  ];

  nativeBuildInputs = [ boost159 boost159.out boost159.dev cmake xorg-libs ];

  postPatch = ''
  ls
  '';

  installPhase = ''
  '';

  meta = with stdenv.lib; {
    description = "Hodini Linux driver";
    homepage = https://github.com/peterwilli/Hodini;
  };
}