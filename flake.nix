{
  description = "Some very cool graphics experiments";

  inputs = {
    nixpkgs = {
      url = "github:NixOS/nixpkgs/nixos-unstable";
    };
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      packages.${system}.default = pkgs.clangStdenv.mkDerivation rec {
        name = "gpu-gems";
        buildInputs = with pkgs; [
          libxkbcommon
          xorg.libX11
          xorg.libXrandr
          xorg.libXcursor
          xorg.libXi
          wayland
          libGL
          libglvnd
          glfw
          glew
          glm
        ];
        nativeBuildInputs = with pkgs; [
          neovim
          cmake
          clang-tools
        ];
        LD_LIBRARY_PATH = "${pkgs.lib.makeLibraryPath buildInputs}";
        CPATH = pkgs.lib.makeSearchPathOutput "dev" "include" buildInputs;
        src = self;
        buildPhase = ''
          cmake .
          cmake --build . --target gpu-gems
        '';
        installPhase = ''
          mkdir -p $out/bin
          mv gpu-gems $out/bin
        '';
      };
    };
}
