
The following isn't likely to help you, but here's some various things I've
learned working on this project.

The gui suffix is appended to files that exist in other directories with the
same name. I was getting some odd link time warnings, and this was an easy
obvious solution.

The 2016 MX-5 uses the ffmpeg-1.1 release for it's libav... libs
