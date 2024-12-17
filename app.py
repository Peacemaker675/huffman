from flask import Flask, redirect, render_template, request, send_file, url_for
import os
import glob


app = Flask(__name__)

UPLOAD_FOLDER = 'uploads'
PROCESSED_FOLDER = 'processed'

for folder in [UPLOAD_FOLDER, PROCESSED_FOLDER]:
    if not os.path.exists(folder):
        os.makedirs(folder)

@app.route("/")
def index():
	files = glob.glob("uploads/*")
	for file in files:
		os.remove(file)
	files = glob.glob("processed/*")
	for file in files:
		os.remove(file)

	return render_template("index.html")

@app.route("/compress", methods=["POST","GET"])
def compress():
	if request.method == "POST":
		file = request.files.get("file")
		if file:
			filename = file.filename
			file_path = os.path.join(UPLOAD_FOLDER, filename)
			file.save(file_path)
			os.system(f"huffman_compress uploads/{filename}")
			filename = filename[:filename.index(".",1)]
			os.system(f"mv uploads/{filename}-compressed.bin processed")
			final_path = f"processed/{filename}-compressed.bin"
			return send_file(final_path,as_attachment = True)
	else:
		return render_template("compress.html",check = 0)

@app.route("/decompress", methods=["POST","GET"])
def decompress():
	if request.method == "POST":
		file = request.files.get("file")
		if file:
			filename = file.filename
			file_path = os.path.join(UPLOAD_FOLDER, filename)
			file.save(file_path)
			os.system(f"huffman_decompress uploads/{filename}")
			filename = filename[:filename.index(".",1)]
			os.system(f"mv uploads/{filename}-decompressed.txt processed")
			final_path = f"processed/{filename}-decompressed.txt"
			return send_file(final_path,as_attachment = True)
	else:
		return render_template("decompress.html",check = 0)
