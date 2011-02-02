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

import java.lang.Math;
import java.io.IOException;
import java.util.ArrayList;
import java.io.DataOutputStream;
import java.io.FileOutputStream;
import java.io.File;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.EOFException;

public class Uncompress {
	public static final int COMPRESS_SIZE = 7;
	public static final int UNCOMPRESS_SIZE = 8;
	public static final byte MSB_CLEAR = 127;
	
	public static void main (String[] args) {
		ArrayList<Byte> input = new ArrayList<Byte>();
		DataInputStream in;
		DataOutputStream out;
		long size = -1;
		int ndx = 0, ctr = 0;
		Byte [] decompress, compress, byteBlock = new Byte[8];

		if (args.length != 2) {
			System.out.println("Usage: Uncompress fromFile toFile");
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

			size = in.readLong();

			try {
				for (ndx = 0; ndx < Math.ceil((size * (COMPRESS_SIZE * 1.0)) 
				 / (UNCOMPRESS_SIZE * 1.0)); ndx++) {
					try {
						input.add(in.readByte());
					}
					catch (EOFException eof) {
						System.out.println("Incorrect file length" +
						 " in compressed data.");
						break;
					}
				}
				in.readByte();
				System.out.println("Incorrect file length" + 
				 " in compressed data.");
				return;
			}
			catch (EOFException eof) {}

			catch (IOException e) {
				System.out.println ( "IO Exception =: " + e );
			}

			while (input.size() % COMPRESS_SIZE != 0)
				input.add((byte)(0));

			compress = new Byte[input.size()];
			decompress = new Byte[(input.size() *
			 UNCOMPRESS_SIZE) / COMPRESS_SIZE];
			compress = input.toArray(compress);

			for (ndx = 0; ndx < compress.length; ndx += COMPRESS_SIZE) {
				System.arraycopy(compress, ndx, byteBlock, 0, COMPRESS_SIZE);
				byteBlock[COMPRESS_SIZE] = 0;
				for (ctr = COMPRESS_SIZE; ctr > 0; ctr--)
					byteBlock[ctr] = (byte) ( ((byteBlock[ctr] >>> (ctr + 1)) 
					 & (int)(Math.pow(2, COMPRESS_SIZE - ctr) - 1)) 
					 | (byteBlock[ctr - 1] << (COMPRESS_SIZE-ctr))
					 & MSB_CLEAR );

				byteBlock[0] = (byte)(byteBlock[0]>>>1 & MSB_CLEAR);
				System.arraycopy(byteBlock, 0, decompress, ((ndx *
				 UNCOMPRESS_SIZE) / COMPRESS_SIZE), UNCOMPRESS_SIZE);
			}

			for (int i=0; i < size; i++)
				out.writeByte(decompress[i]);

			out.close ();
			in.close ();
		}

		catch (EOFException eof) {}

		catch (IOException e) {
			System.out.println ( "IO Exception =: " + e );
		}
	}
}
