{
    description = "C nix dev environment";

    inputs = {
        nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    };

    outputs = { self, nixpkgs }:
        let
            system = "x86_64-linux";
            pkgs = import nixpkgs { inherit system; };
        in
        {
            devShells.${system}.default = pkgs.mkShell {
              packages = [ pkgs.gcc pkgs.clang-tools pkgs.linux-manual pkgs.man-pages pkgs.man-pages-posix ]; 



              shellHook = ''
                    export MANPATH="$MANPATH='' + (pkgs.lib.makeSearchPath "share/man" [ pkgs.man-pages pkgs.man-pages-posix ]) + ''"
                    export SHELL=/run/current-system/sw/bin/bash
                    '';
            };
        };
}
