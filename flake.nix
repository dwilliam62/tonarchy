{
  description = "tonarchy - Minimal Arch Linux installer";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" ];
      forAllSystems = fn: nixpkgs.lib.genAttrs systems (system: fn nixpkgs.legacyPackages.${system});
    in
    {
      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          buildInputs = [
            pkgs.gcc
            pkgs.gnumake
            pkgs.ncurses.dev
            pkgs.notcurses
            pkgs.pkg-config
            pkgs.bear
          ];
          shellHook = ''
            export PS1="(tonarchy-dev) $PS1"
            echo "tonarchy development environment"
            echo "Run 'make' to build"
            echo "Run 'make -f Makefile.notcurses && ./notcurses_demo' to see notcurses demo"
          '';
        };
      });
    };
}
