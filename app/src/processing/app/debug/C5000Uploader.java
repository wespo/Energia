/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  ************************************************************************
  *	C5000Uploader.java
  *
  *	Abstract uploading baseclass for C5000
  *		Copyright (c) 2013 Texas Instruments. All right reserved.
  *
  *
  ***********************************************************************
  Derived from:
  Uploader - abstract uploading baseclass (common to both uisp and avrdude)
  Part of the Arduino project - http://www.arduino.cc/

  Copyright (c) 2004-05
  Hernando Barragan

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  $Id$
*/
package processing.app.debug;

import processing.app.Base;
import processing.app.Preferences;
import processing.app.Serial;
import processing.app.SerialException;

import java.io.*;
import java.util.*;
import java.util.zip.*;
import javax.swing.*;
import gnu.io.*;

public class C5000Uploader extends Uploader{
	public C5000Uploader() {
	}
	public boolean uploadUsingPreferences(String buildPath, String className, boolean usingProgrammer)
	throws RunnerException, SerialException {
		this.verbose = verbose;
		Map<String, String> boardPreferences = Base.getBoardPreferences();
		String basePath = Base.getBasePath();
		Collection params = new ArrayList();

		//params.add("-p" + "c55xx");
		//params.add("-C" + basePath + File.separator + "avrdude.conf");
		params.add(buildPath + File.separator + "bootimg.bin");
		params.add(Preferences.get("serial.port"));

		return c5000avrdude(params);
	}

	public boolean burnBootloader() throws RunnerException {
		//nothing do do for C5000
		return false;
	}

	public boolean c5000avrdude(Collection params) throws RunnerException {
		List commandDownloader = new ArrayList();

		if ( Base.isLinux() || Base.isMacOS() ) {
			commandDownloader.add(Base.getBasePath() + "pydude"); //was avrdude
		}
		else {
			commandDownloader.add(Base.getBasePath() + "pydude.exe"); //was avrdude
		}
		commandDownloader.addAll(params);

		return executeUploadCommand(commandDownloader);
	}
}
