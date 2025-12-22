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
            pkgs.glibc.static
            pkgs.gnumake
            pkgs.bear
            pkgs.qemu_kvm
            pkgs.OVMF
          ];
          shellHook = ''
            export PS1="(tonarchy-dev) $PS1"
            echo "tonarchy development environment"
            echo "Run 'make' to build"
            echo "Run './vm-test [iso-path]' to test an ISO in qemu"
          '';
        };
      });
    };
}
