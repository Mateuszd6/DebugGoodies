"""
My texture viewer for examining textures in GDB. Horriebly buggy. Work in
infinite progress. It uses feh to display images (as an external program),
because i like it far more than PIL's default image interface.
"""


import os
import numpy
from PIL import Image


def MakeAndDisplayTexture(pix, w, h):
    """ Make an image from the pixel array and siplay it using feh. """
    mat = []
    for y in range(0, h):
        row = []
        for x in range(0, w):
            blue = pix[4 * (y * w + x) + 0]
            green = pix[4 * (y * w + x) + 1]
            red = pix[4 * (y * w + x) + 2]
            alpha = pix[4 * (y * w + x) + 3]

            # NOTE: Alpha is not supported at this time.
            p = 0xFF000000 + red + green * 256 + blue * (256 ** 2)
            row.append(p)
            mat.append(row)

    numpy_mat = numpy.array(mat, numpy.uint32)
    img = Image.fromarray(numpy_mat, 'RGBA')
    img.save('/home/mateusz/my.png')


class DisplayRawTexture (gdb.Command):
    """
    Given a pointer to unsigned char *, width and height examine the texture.
    """

    def __init__ (self):
        super (DisplayRawTexture, self).__init__ ("display-texture",
                                                  gdb.COMMAND_USER)

    def invoke (self, args, from_tty):
        argv = gdb.string_to_argv (args)
        if len (argv) != 3:
            # TODO: Usage
            raise gdb.GdbError ("hello-world takes 3 argumens ")

        pix = gdb.parse_and_eval(argv[0])
        w = gdb.parse_and_eval(argv[1])
        h = gdb.parse_and_eval(argv[2])

        MakeAndDisplayTexture(pix, w, h)


class DisplaySDLSurface (gdb.Command):
    """
    Given a pointer to SDL_Surface, examine the texture.
    """

    def __init__ (self):
        super (DisplaySDLSurface, self).__init__ ("display-sdl-surface",
                                           gdb.COMMAND_USER)

    def invoke (self, args, from_tty):
        argv = gdb.string_to_argv (args)
        if len (argv) != 1:
            # TODO: Usage
            raise gdb.GdbError ("display-sdl-surface takes 1 argument")

        w = gdb.parse_and_eval('*(int *)((void *)%s + 16)' % (argv[0]))
        h = gdb.parse_and_eval('*(int *)((void *)%s + 20)' % (argv[0]))
        pix = gdb.parse_and_eval('*(unsigned char **)((void *)%s + 32)'
                                 % (argv[0]))

        MakeAndDisplayTexture(pix, w, h)


# GDB requires that they are initialized here.
HelloWorld()
DisplaySDLSurface()
