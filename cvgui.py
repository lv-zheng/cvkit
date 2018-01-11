#!/usr/bin/python3

import os
import subprocess
import sys
import tempfile

import tkinter
import tkinter.filedialog
import tkinter.messagebox

class CVkit(tkinter.Frame):
    def __init__(self):
        super().__init__()
        self.initUI()
        self.fn = None
        self.img = None
        self.cvshowproc = None
        self.otherproc = []
        self.tmpfiles = []

    def __del__(self):
        self.cleanup()

    def initUI(self):
        self.master.title("CVkit")
        self.pack(fill=tkinter.BOTH, expand=1)

        menubar = tkinter.Menu(self.master)
        self.master.config(menu=menubar)

        filemenu = tkinter.Menu(menubar)
        filemenu.add_command(label="Open", command=self.onOpen)
        filemenu.add_command(label="Show", command=self.showimg)
        filemenu.add_command(label="Show Another", command=self.onShowAno)
        filemenu.add_command(label="Save As", command=self.onSave)
        filemenu.add_command(label="Quit", command=self.onQuit)

        colormenu = tkinter.Menu(menubar)
        graymenu = tkinter.Menu(colormenu)
        graymenu.add_command(label="red channel", command=self.on2gray_r)
        graymenu.add_command(label="green channel", command=self.on2gray_g)
        graymenu.add_command(label="blue channel", command=self.on2gray_b)
        graymenu.add_command(label="average", command=self.on2gray_a)
        graymenu.add_command(label="luminosity (best)", command=self.on2gray_l)
        colormenu.add_cascade(label="convert to grayscale", menu=graymenu)

        thresmenu = tkinter.Menu(menubar)
        thresmenu.add_command(label="Otsu", command=self.onOtsu)

        arithmenu = tkinter.Menu(menubar)
        arithmenu.add_command(label="add", command=self.onAdd)
        arithmenu.add_command(label="subtract", command=self.onSub)
        arithmenu.add_command(label="multiply", command=self.onMul)

        scalemenu = tkinter.Menu(menubar)
        scalemenu.add_command(label="nearest neighbor (scale x)", command=self.onScale_nn)
        scalemenu.add_command(label="bilinear (scale x)", command=self.onScale_bl)

        contrastmenu = tkinter.Menu(menubar)
        contrastmenu.add_command(label="histogram", command=self.onHis)
        contrastmenu.add_command(label="histogram equalize (lo,hi)", command=self.onHisEq)

        blurmenu = tkinter.Menu(menubar)
        blurmenu.add_command(label="average ([r]adius)", command=self.onBlur_avg)
        blurmenu.add_command(label="median ([r]adius)", command=self.onBlur_med)
        blurmenu.add_command(label="Gaussian (σ)", command=self.onBlur_gau)

        edgemenu = tkinter.Menu(menubar)
        edgemenu.add_command(label="Sobel", command=self.onEdge_sob)
        edgemenu.add_command(label="Laplacian", command=self.onEdge_lap)
        edgemenu.add_command(label="Negative Laplacian", command=self.onEdge_neglap)
        edgemenu.add_command(label="Canny (threshold)", command=self.onEdge_canny)

        morphmenu = tkinter.Menu(menubar)

        binmorphmenu = tkinter.Menu(morphmenu)
        binmorphmenu.add_command(label="erosion", command=self.onBM_ero)
        binmorphmenu.add_command(label="dilation", command=self.onBM_dil)
        binmorphmenu.add_command(label="open", command=self.onBM_ope)
        binmorphmenu.add_command(label="close", command=self.onBM_clo)
        morphmenu.add_cascade(label="binary", menu=binmorphmenu)

        graymorphmenu = tkinter.Menu(morphmenu)
        graymorphmenu.add_command(label="erosion ([r]adius)", command=self.onGM_ero)
        graymorphmenu.add_command(label="dilation ([r]adius)", command=self.onGM_dil)
        graymorphmenu.add_command(label="open ([r]adius)", command=self.onGM_ope)
        graymorphmenu.add_command(label="close ([r]adius)", command=self.onGM_clo)
        morphmenu.add_cascade(label="grayscale", menu=graymorphmenu)

        menubar.add_cascade(label="File", menu=filemenu)
        menubar.add_cascade(label="Color", menu=colormenu)
        menubar.add_cascade(label="Threshold", menu=thresmenu)
        menubar.add_cascade(label="Arithmetic", menu=arithmenu)
        menubar.add_cascade(label="Scale", menu=scalemenu)
        menubar.add_cascade(label="Contrast", menu=contrastmenu)
        menubar.add_cascade(label="Blur", menu=blurmenu)
        menubar.add_cascade(label="Edge", menu=edgemenu)
        menubar.add_cascade(label="Morph", menu=morphmenu)

        frame1 = tkinter.Frame(self)
        frame1.pack(fill=tkinter.X)
        lbl1 = tkinter.Label(frame1, text="lo", width=6)
        lbl1.pack(side=tkinter.LEFT, padx=5, pady=5)
        scale1 = tkinter.Scale(frame1, from_=0, to_=255, orient=tkinter.HORIZONTAL)
        scale1.pack(fill=tkinter.X, padx=5, expand=True)
        scale1.set(50)
        self.scale_lo = scale1

        frame2 = tkinter.Frame(self)
        frame2.pack(fill=tkinter.X)
        lbl2 = tkinter.Label(frame2, text="hi", width=6)
        lbl2.pack(side=tkinter.LEFT, padx=5, pady=5)
        scale2 = tkinter.Scale(frame2, from_=0, to_=255, orient=tkinter.HORIZONTAL)
        scale2.pack(fill=tkinter.X, padx=5, expand=True)
        scale2.set(200)
        self.scale_hi = scale2

        frame3 = tkinter.Frame(self)
        frame3.pack(fill=tkinter.X)
        lbl3 = tkinter.Label(frame3, text="scale x", width=6)
        lbl3.pack(side=tkinter.LEFT, padx=5, pady=5)
        scale3 = tkinter.Scale(frame3, from_=0.1, to_=8, orient=tkinter.HORIZONTAL, resolution=0.05)
        scale3.pack(fill=tkinter.X, padx=5, expand=True)
        scale3.set(0.5)
        self.scale_x = scale3

        frame4 = tkinter.Frame(self)
        frame4.pack(fill=tkinter.X)
        lbl4 = tkinter.Label(frame4, text="r/σ", width=6)
        lbl4.pack(side=tkinter.LEFT, padx=5, pady=5)
        scale4 = tkinter.Scale(frame4, from_=0.5, to_=8, orient=tkinter.HORIZONTAL, resolution=0.05)
        scale4.pack(fill=tkinter.X, padx=5, expand=True)
        scale4.set(3)
        self.scale_r = scale4

        frame5 = tkinter.Frame(self)
        frame5.pack(fill=tkinter.X)
        lbl5 = tkinter.Label(frame5, text="thres", width=6)
        lbl5.pack(side=tkinter.LEFT, padx=5, pady=5)
        scale5 = tkinter.Scale(frame5, from_=0, to_=1, orient=tkinter.HORIZONTAL, resolution=0.01)
        scale5.pack(fill=tkinter.X, padx=5, expand=True)
        scale5.set(0.2)
        self.scale_canny = scale5

    def onOpen(self):
        self.closeimg()
        dlg = tkinter.filedialog.Open()
        fn = dlg.show()
        if fn == "":
            self.fn = None
            return False
        print(fn)
        self.fn = fn
        self.showimg()
        return True

    def onShowAno(self):
        dlg = tkinter.filedialog.Open(title="Choose another file")
        file2 = dlg.show()
        if file2 == "":
            return
        self.show_another(file2)

    def onSave(self):
        if self.fn is None:
            self.showerr()
            return

        ftypes = [('PNG', '*.png'),
                  ('TIFF', '*.tif'),
                  ('JPEG (not recommended for intermediate results)', '*.jpg')]
        dlg = tkinter.filedialog.SaveAs(filetypes=ftypes)
        fnsave = dlg.show()

        if fnsave == "":
            return

        args = []
        args.append(sys.path[0] + "/cvcvt")
        args.append(self.fn)
        args.append(fnsave)

        proc = subprocess.run(args,
                stderr=subprocess.PIPE, universal_newlines=True)
        if proc.returncode != 0:
            self.showerr(proc.stderr)
            return

    def onQuit(self):
        self.cleanup()
        sys.exit()

    def on2gray_r(self):
        self.togray('r')
    def on2gray_g(self):
        self.togray('g')
    def on2gray_b(self):
        self.togray('b')
    def on2gray_a(self):
        self.togray('a')
    def on2gray_l(self):
        self.togray('l')

    def onOtsu(self):
        self.do_and_switch(["cvthreshold_otsu"])

    def onAdd(self):
        self.arith('a')
    def onSub(self):
        self.arith('s')
    def onMul(self):
        self.arith('m')

    def onScale_nn(self):
        x = self.scale_x.get()
        self.do_and_switch(["cvscale", 'x' + str(x)])
    def onScale_bl(self):
        x = self.scale_x.get()
        self.do_and_switch(["cvscale", 'x' + str(x), 'b'])

    def onHis(self):
        hisfn = self.do_and_switch(["cvhistogram"], False)
        if hisfn is not None:
            self.show_another(hisfn)

    def onHisEq(self):
        lo = self.scale_lo.get()
        hi = self.scale_hi.get()
        if lo < 0 or lo >= hi or hi > 255:
            self.showerr("lo and hi are not in range ([0, 255], lo < hi)")
            return
        self.do_and_switch(["cvequal", str(lo), str(hi)])

    def onBlur_avg(self):
        r = self.scale_r.get()
        if r != int(r):
            self.showerr("radius should be an integer")
            return
        self.do_and_switch(["cvblur", "avg", str(r)])
    def onBlur_med(self):
        r = self.scale_r.get()
        if r != int(r):
            self.showerr("radius should be an integer")
            return
        self.do_and_switch(["cvblur", "med", str(r)])
    def onBlur_gau(self):
        r = self.scale_r.get()
        self.do_and_switch(["cvblur", "gau", str(r)])

    def onEdge_sob(self):
        self.do_and_switch(["cvedge", "sob"])
    def onEdge_lap(self):
        self.do_and_switch(["cvedge", "lap"])
    def onEdge_neglap(self):
        self.do_and_switch(["cvedge", "neglap"])
    def onEdge_canny(self):
        thres = self.scale_canny.get()
        self.do_and_switch(["cvedge", "canny", str(thres)])

    def onBM_ero(self):
        self.binmorph("ero")
    def onBM_dil(self):
        self.binmorph("dil")
    def onBM_ope(self):
        self.binmorph("ope")
    def onBM_clo(self):
        self.binmorph("clo")

    def onGM_ero(self):
        self.graymorph("ero")
    def onGM_dil(self):
        self.graymorph("dil")
    def onGM_ope(self):
        self.graymorph("ope")
    def onGM_clo(self):
        self.graymorph("clo")

    def binmorph(self, method):
        dlg = tkinter.filedialog.Open(title="Choose the structuring element")
        file2 = dlg.show()
        if file2 == "":
            return
        self.do_and_switch(["cvbinmorph", method, file2])

    def graymorph(self, method):
        r = self.scale_r.get()
        if r != int(r):
            self.showerr("radius should be an integer")
            return
        self.do_and_switch(["cvgraymorph", method, str(r)])

    def getarg(idx):
        pass

    def show_another(self, fn2):
        self.otherproc.append(subprocess.Popen([
                sys.path[0] + "/cvshow",
                fn2],
                stderr = subprocess.PIPE))

    def tmpfile(self):
        tup = tempfile.mkstemp(suffix=".png", prefix="cvgui_")
        os.close(tup[0])
        print(tup[1])
        self.tmpfiles.append(tup[1])
        return tup[1]

    def do_and_switch(self, args, switch=True):
        if self.fn is None:
            self.showerr()
            return None

        tmp = self.tmpfile()

        rargs = []
        rargs.append(sys.path[0] + "/" + args[0])
        rargs.append(self.fn)
        rargs.append(tmp)
        rargs.extend(args[1:])

        proc = subprocess.run(rargs,
                stderr=subprocess.PIPE, universal_newlines=True)
        if proc.returncode != 0:
            self.showerr(proc.stderr)
            return None

        if switch:
            self.switchfile(tmp)

        return tmp

    def togray(self, method):
        self.do_and_switch(["cvcolor2gray", method])

    def arith(self, method):
        dlg = tkinter.filedialog.Open(title="Choose another file")
        file2 = dlg.show()
        if file2 == "":
            return
        self.do_and_switch(["cvarith", method, file2])

    def switchfile(self, fn):
        self.fn = fn
        self.showimg()

    def showimg(self):
        self.closeimg()

        if self.fn is None:
            self.showerr()
            return

        validproc = subprocess.run([sys.path[0] + "/cvvalid", self.fn], stderr=subprocess.PIPE, universal_newlines=True)
        if validproc.returncode != 0:
            self.showerr(validproc.stderr)
            self.fn = None
            return

        self.cvshowproc = subprocess.Popen([
                sys.path[0] + "/cvshow",
                self.fn],
                stderr = subprocess.PIPE)

    def showerr(self, err = None):
        if err is None:
            errstr = "Please open an image first"
        else:
            errstr = err
        tkinter.messagebox.showerror("Error", errstr)

    def closeimg(self):
        if self.cvshowproc is None:
            return
        if self.cvshowproc.poll() is None:
            self.cvshowproc.terminate()

    def closeproc(self):
        self.closeimg()
        for proc in self.otherproc:
            if proc.poll() is None:
                proc.terminate()

    def cleanup(self):
        self.closeproc()
        for f in self.tmpfiles:
            os.unlink(f)

def main():
    root = tkinter.Tk()
    root.geometry("480x270")
    app = CVkit()
    root.mainloop()

if __name__ == '__main__':
    main()
