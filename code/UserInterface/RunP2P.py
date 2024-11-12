import tkinter as tk
from tkinter import messagebox
import subprocess

def run_peer():
    
    port = port_entry.get()
    ip = ip_entry.get()
    index_ip = index_ip_entry.get()
    index_port = index_port_entry.get()
    directory = directory_entry.get()
    if not port or not ip or not index_ip or not index_port or not directory:
        messagebox.showwarning("Empty Fields", "Please fill in all fields.")
        return

    try:
        
        process = subprocess.Popen(
            ['../PeerClient/a.out', port, ip, index_ip, index_port, directory],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        output, error = process.communicate()

        if error:
            raise Exception(f"Error in RunPeer.cpp: {error.decode()}")

        
        output_text.delete(1.0, tk.END)
        output_text.insert(tk.END, output.decode())

        
        search_file_frame.pack()

    except Exception as e:
        messagebox.showerror("Error", f"Could not execute RunPeer.cpp: {str(e)}")


def search_file():
    
    filename = filename_entry.get()
    if not filename:
        messagebox.showwarning("Empty Field", "Please enter a file name.")
        return

    try:
        
        process = subprocess.Popen(
            ['../PeerClient/FileFinder/a.out', filename],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        output, error = process.communicate()

        if error:
            raise Exception(f"Error in FileFinder.cpp: {error.decode()}")

        search_output_text.delete(1.0, tk.END)
        search_output_text.insert(tk.END, output.decode())

    except Exception as e:
        messagebox.showerror("Error", f"Could not execute FileFinder.cpp: {str(e)}")

