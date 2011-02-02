/*
 * Copyright (c) 2009 Murtaza Munaim
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

import java.util.ArrayList;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public class Compress {
	public static final int UNCOMPRESS_SIZE = 8;
	public static final int COMPRESS_SIZE = 7;
	public static final int INVALID_VALUE = 256;
	
	public static void main(String[] args) {

		DataInputStream in;
		DataOutputStream out; 
		ArrayList<Byte> input = new ArrayList<Byte> ();
		Byte [] bitFlip = new Byte[UNCOMPRESS_SIZE], inputText, compress;
		int ndx = 0, ctr = 0;
		boolean error = false;
		byte singleByte = 0;
		long size = 0;

		if (args.length != 2) {
			System.out.println("Usage: Compress fromFile toFile");
			System.exit(0);
		}

		try {
			File inFile = new File(args[0]), outFile = new File(args[1]);

			if (outFile.exists()) {
				System.out.println("Cannot create " + outFile.getName() + 
				 ", perhaps because it already exists.");
				System.exit(0);
			}
			if (!inFile.canRead()) {
				System.out.println("Input file " + inFile.getName() + 
				 " doesn't exist or is unreadable.");
				System.exit(0);
			}

			out = new DataOutputStream(new FileOutputStream(outFile));
			in = new DataInputStream(new FileInputStream(inFile));

			try {
				while ((singleByte = in.readByte()) >= 0) {
					input.add(singleByte);
				}
				if (singleByte < 0) {
					System.out.println("Input file includes nontext byte: " + 
					 (int)(singleByte + INVALID_VALUE ));
					input.clear();
					error = true;
				}
			}				
			catch (EOFException eof) {}

			size = (long) input.size();
			while ( (input.size() % UNCOMPRESS_SIZE) != 0)
				input.add((byte) 0);

			inputText = new Byte[input.size()];
			compress = 
			 new Byte[(input.size() * COMPRESS_SIZE) / UNCOMPRESS_SIZE];
			inputText = input.toArray(inputText);

			for( ndx = 0; ndx < inputText.length; ndx += UNCOMPRESS_SIZE) {
				System.arraycopy(inputText, ndx, bitFlip, 0, UNCOMPRESS_SIZE);
				for (ctr = 0; ctr < COMPRESS_SIZE; ctr++)
					bitFlip[ctr] = (byte) ((bitFlip[ctr] << (ctr+1))
					 | (bitFlip[ctr+1] >>> ((COMPRESS_SIZE - 1) - ctr)));			
				System.arraycopy(bitFlip, 0, compress, 
				 (ndx * COMPRESS_SIZE) / UNCOMPRESS_SIZE , COMPRESS_SIZE);
			}
			
			if (!error )
				out.writeLong(size);
			for (ndx = 0; ndx < compress.length && compress[ndx] != 0; ndx++)
				out.writeByte(compress[ndx]);

			in.close();
			out.close();

		} 
		catch (IOException e) {
			System.out.println("I/O Error: " + e.getMessage());
		} 
		catch (Exception e) {
			e.printStackTrace();
		}
	}
}
