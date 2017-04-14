#!/bin/bash -x

sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/altgr select The default for the keyboard layout"
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/compose select No compose key"
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/ctrl_alt_bksp boolean false"
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/layoutcode string de"
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/layout select German"
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/modelcode string pc105"
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/model select Generic 105-key (Intl) PC"
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/optionscode string "
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/store_defaults_in_debconf_db boolean true"
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/switch select No temporary switch"
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/toggle select No toggling"
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/unsupported_config_layout boolean true"
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/unsupported_config_options boolean true"
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/unsupported_layout boolean true"
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/unsupported_options boolean true"
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/variantcode string "
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/variant select German"
sudo debconf-set-selections <<< "keyboard-configuration keyboard-configuration/xkb-keymap select "

sudo mkdir -p /etc/X11/xorg.conf.d
echo | sudo tee /etc/X11/xorg.conf.d/xorg.conf <<EOF
Section "Device"
    Identifier  "Configured Video Device"
    Driver      "dummy"
EndSection

Section "Monitor"
    Identifier  "Configured Monitor"
    HorizSync 31.5-48.5
    VertRefresh 50-70
EndSection

Section "Screen"
    Identifier  "Default Screen"
    Monitor     "Configured Monitor"
    Device      "Configured Video Device"
    DefaultDepth 24
    SubSection "Display"
    Depth 24
    Modes "1024x800"
    EndSubSection
EndSection
