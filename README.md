# brightnessctl

This is a fork of [Hummer's brightnessctl](https://github.com/Hummer12007/brightnessctl).

This program allows you read and control device brightness on Linux. Devices, by default, include backlight and LEDs (searched for in corresponding classes). If omitted, the first found device is selected.

It can also preserve current brightness before applying the operation (allowing for usecases like disabling backlight on lid close).

## Installation

> [!NOTE]
> To run **this** fork you have to build it yourself or use the nix overlay.

The *original* program is available in:
* [Alpine Linux](https://pkgs.alpinelinux.org/packages?name=brightnessctl&branch=edge)
* [Arch Linux](https://www.archlinux.org/packages/extra/x86_64/brightnessctl/)
* [Void Linux](https://github.com/void-linux/void-packages/blob/master/srcpkgs/brightnessctl/template)
* [Debian](https://packages.debian.org/testing/source/brightnessctl) - starting with Buster (and derivatives)
* [Ubuntu](https://packages.ubuntu.com/search?suite=all&searchon=names&keywords=brightnessctl) - starting with 18.04 (and derivatives)
* [openSUSE](https://build.opensuse.org/package/show/utilities/brightnessctl) - available in Tumbleweed, use OBS `utilities/brightnessctl` devel project for Leap < 15.1
* [Fedora](https://src.fedoraproject.org/rpms/brightnessctl) - available in Fedora 31+
* [NixOS/nix](https://search.nixos.org/packages?type=packages&query=brightnessctl) - starting with 17.09, please see the [NixOS Wiki page](https://wiki.nixos.org/wiki/Backlight#brightnessctl) for the "best-practice" configuration file based installation

## Build instructions

To build the project run `./configure && make`.
Consider `./configure --help` for relevant options.

A minimal environment can be created with `nix-shell -p coreutils pkg-config systemd`.

> [!NOTE]
> To enable systemd (recommended) run `./configure --enable-logind`.

## Nix Overlay

Replace `...` with the corresponding version/hash you want.
`rev` is the commit hash. If you're having trouble with the hash, set it to `""` and copy the correct one from the error message.

```nix
final: prev: {
  brightnessctl = prev.brightnessctl.overrideAttrs (old: {
    version = "...";
    src = prev.fetchFromGitHub {
      owner = "Severin-Nitsche";
      repo = "brightnessctl";
      rev = "...";
      hash = "...";
    };
    
    makeFlags = [
      "PREFIX="
      "DESTDIR=$(out)"
    ];

    postPatch = ''
      substituteInPlace 90-brightnessctl.rules \
        --replace-fail /bin/ ${prev.coreutils}/bin/

      substituteInPlace configure \
        --replace-fail "pkg-config" "$PKG_CONFIG"
    '';

    configureFlags = [
      "--enable-logind"
      "--prefix="
    ];
  });
}
```

*Consider [my dotfiles](https://github.com/Severin-Nitsche/dotfiles/blob/6ac3b8c81e7f1224608754bfbdbfafbcd01d20bf/overlays/default.nix#L9) for reference*

## Permissions

Modifying brightness requires write permissions for device files or systemd support. `brightnessctl` accomplishes this (without using `sudo`/`su`/etc.) by either of the following means:

1) installing relevant udev rules to add permissions to backlight class devices for users in `video` and leds for users in `input`. This requires your user to be in the `video` and `input` groups. (done by default)

2) installing `brightnessctl` as a suid binary.

3) using the `systemd-logind` API.

## FAQ

#### I'd like to configure the brightness of an external monitor

Use the [ddcci-driver-linux](https://gitlab.com/ddcci-driver-linux/ddcci-driver-linux) kernel module to expose external monitor brightness controls to `brightnessctl`. Available in repositories of [AUR](https://aur.archlinux.org/packages/ddcci-driver-linux-dkms/), [Debian](https://packages.debian.org/stable/ddcci-dkms), [Nix](https://github.com/NixOS/nixpkgs/blob/master/pkgs/os-specific/linux/ddcci/default.nix), [Ubuntu](https://packages.ubuntu.com/search?suite=all&searchon=names&keywords=ddcci-dkms), [Void](https://github.com/void-linux/void-packages/tree/master/srcpkgs/ddcci-dkms).

## Usage

Take a look at `brightnessctl --help`.
There is also a manpage available.
