#!/usr/bin/env python3
"""Fills the table of contents of the manual and writes .docx and .pdf.

   python3 doc/manual/finish.py doc/ooRegina-Manual.docx

Starts a headless LibreOffice, updates all indexes (the contents with page
numbers) and stores the document as Word file and as PDF next to it.
"""
import os
import subprocess
import sys
import tempfile
import time
from pathlib import Path

import uno
from com.sun.star.beans import PropertyValue


def prop(name, value):
    p = PropertyValue()
    p.Name, p.Value = name, value
    return p


def main(src):
    src = Path(src).resolve()
    profile = tempfile.mkdtemp(prefix="lo_profile_")
    env = dict(os.environ, SAL_USE_VCLPLUGIN="svp")
    port = 2002 + os.getpid() % 1000
    office = subprocess.Popen(
        ["soffice", f"-env:UserInstallation=file://{profile}", "--headless", "--invisible",
         "--nologo", "--norestore",
         f"--accept=socket,host=127.0.0.1,port={port};urp;StarOffice.ComponentContext"],
        env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        local = uno.getComponentContext()
        resolver = local.ServiceManager.createInstanceWithContext(
            "com.sun.star.bridge.UnoUrlResolver", local)
        for _ in range(60):
            try:
                ctx = resolver.resolve(
                    f"uno:socket,host=127.0.0.1,port={port};urp;StarOffice.ComponentContext")
                break
            except Exception:
                time.sleep(1)
        else:
            raise SystemExit("LibreOffice did not start")
        desktop = ctx.ServiceManager.createInstanceWithContext("com.sun.star.frame.Desktop", ctx)
        doc = desktop.loadComponentFromURL(src.as_uri(), "_blank", 0, (prop("Hidden", True),))
        # twice: the contents itself takes pages, which moves the page numbers
        for _ in range(2):
            idx = doc.getDocumentIndexes()
            for i in range(idx.getCount()):
                idx.getByIndex(i).update()
            doc.refresh()
        doc.storeToURL(src.as_uri(), (prop("FilterName", "MS Word 2007 XML"),))
        pdf = src.with_suffix(".pdf")
        doc.storeToURL(pdf.as_uri(), (prop("FilterName", "writer_pdf_Export"),))
        doc.close(True)
        print("wrote", src, "and", pdf)
    finally:
        office.terminate()


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "doc/ooRegina-Manual.docx")
