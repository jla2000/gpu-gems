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
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        name = "gpu-gems";
        nativeBuildInputs = with pkgs; [
          zig
          glfw
          glew
          libGL
        ];
        src = self;
        buildPhase = ''
          export XDG_CACHE_HOME=xdg_cache
          zig build
        '';
        installPhase = ''
          mkdir -p $out/bin
          cp ./zig-out/bin/gpu-gems $out/bin
        '';
      };
    };
}
