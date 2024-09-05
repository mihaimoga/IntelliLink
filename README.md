![IntelliLink.png](IntelliLink.png)

Download:
- [IntelliLinkSetup.msi](https://www.moga.doctor/freeware/IntelliLinkSetup.msi)
- [IntelliLink.zip](https://www.moga.doctor/freeware/IntelliLink.zip)

## Introduction

_IntelliLink_ is a free (as in “free speech” and also as in “free beer”) online link manager. Running in the MS Windows environment, its use is governed by [GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.html). _IntelliLink_ is written in C++ and uses pure Win32 API and STL which ensures a higher execution speed and smaller program size. By optimizing as many routines as possible without losing user friendliness, _IntelliLink_ is trying to reduce the world carbon dioxide emissions. When using less CPU power, the PC can throttle down and reduce power consumption, resulting in a greener environment. I hope you enjoy _IntelliLink_ as much as I enjoy coding it!

## Getting started

### Install IntelliLink using the installer

- Download the installer
- Run the executable binary and follow the installation flow

The installer will likely require Administrative privileges in order to install _IntelliLink_ (and later, to update _IntelliLink_ or install or update plugins, or anything else that requires writing to the installation directory). If you do not have Administrative privileges, you either need to tell the installer to use a location where you do have write permission (though that may still ask for Administrator privileges), or you may choose not use the installer and instead run a portable edition from a directory where you have write permission.

### Install IntelliLink from zip

These instructions will allow you to run a portable or mini-portable (also called “minimalist”), without requiring administrative privileges.

- Create a new folder somewhere that you have write-permission
- Unzip the content into the new folder
- Run _IntelliLink_ from the new folder

For all the portable editions, everything (the application, settings, and plugins) is stored in one folder hierarchy. And by default, the portable editions do not mess with operating system settings (so no file associations or Edit with _IntelliLink_ context-menu entries) – if you want such with a portable edition, you will have to set it up manually.

The portable zip edition contain all the themes, user defined languages, auto-completions, localizations, and default plugins and Plugins Admin tool that come with the installed version of _IntelliLink_.

The portable zip edition of _IntelliLink_ can be removed by deleting the directory they came in. If you manually set up file associations or context-menu entries in the OS, it is your responsibility to remove them yourself.

## Create and Submit your Pull Request

As noted in the [Contributing Rules](https://github.com/mihaimoga/IntelliLink/blob/main/CONTRIBUTING.md) for _IntelliLink_, all Pull Requests need to be attached to a issue on GitHub. So the first step is to create an issue which requests that the functionality be improved (if it was already there) or added (if it was not yet there); in your issue, be sure to explain that you have the functionality definition ready, and will be submitting a Pull Request. The second step is to use the GitHub interface to create the Pull Request from your fork into the main repository. The final step is to wait for and respond to feedback from the developers as needed, until such time as your PR is accepted or rejected.
