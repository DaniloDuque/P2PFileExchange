import tkinter as tk
import customtkinter
from tkinter import messagebox
import subprocess
import threading  

port = 0
IP = 0
indexIp = 0
indexPort = 0
directory = 0

def run_peer():
    global port, IP, indexIp, indexPort, directory
    port = port_entry.get()
    IP = ip_entry.get()
    indexIp = index_ip_entry.get()
    indexPort = index_port_entry.get()
    directory = directory_entry.get()
    server_thread = threading.Thread(target=start_peer_server)
    server_thread.daemon = True  
    server_thread.start()

def start_peer_server():
    global port, IP, indexIp, indexPort, directory
    try:
        process = subprocess.Popen(
            ['../PeerClient/a.out', port, IP, indexIp, indexPort, "../PeerClient/" + directory],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        output, error = process.communicate()
        if error:
            raise Exception(f"Error in RunPeer.cpp: {error.decode()}")
        if output:
            print(output.decode())
        app.after(100, update_output_text, output.decode())
    except Exception as e:
        app.after(100, lambda: messagebox.showerror("Error", f"Could not execute RunPeer.cpp: {str(e)}"))

def search_file(port, indexIp, indexPort, filename, dir="saved"):
    try:
        process = subprocess.Popen(
            ['../PeerClient/FileFinder/a.out', port, indexIp, indexPort, filename, dir],
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

def update_output_text(output):
    output_text.delete(1.0, tk.END)
    output_text.insert(tk.END, output)

def switch_to_second_page():
    run_peer()  
    for widget in app.winfo_children():
        widget.pack_forget()
    second_page()

def initial_page():
    global port_entry, ip_entry, index_ip_entry, index_port_entry, directory_entry
    
    app.title("")
    
    title = customtkinter.CTkLabel(app, text="P2P File Exchange", font=("Ubuntu", 45))
    title.pack(pady=20)

    port_entry = customtkinter.CTkEntry(app, placeholder_text="My Port", corner_radius=15, font=("Ubuntu", 20), width=350, height=70)
    port_entry.pack(pady=10, expand=True)

    ip_entry = customtkinter.CTkEntry(app, placeholder_text="My IP Address", corner_radius=15, font=("Ubuntu", 20), width=350, height=70)
    ip_entry.pack(pady=10, expand=True)

    index_ip_entry = customtkinter.CTkEntry(app, placeholder_text="Index Server IP", corner_radius=15, font=("Ubuntu", 20), width=350, height=70)
    index_ip_entry.pack(pady=10, expand=True)

    index_port_entry = customtkinter.CTkEntry(app, placeholder_text="Index Server Port", corner_radius=15, font=("Ubuntu", 20), width=350, height=70)
    index_port_entry.pack(pady=10, expand=True)

    directory_entry = customtkinter.CTkEntry(app, placeholder_text="Shared Directory", corner_radius=15, font=("Ubuntu", 20), width=350, height=70)
    directory_entry.pack(pady=10, expand=True)

    submit_button = customtkinter.CTkButton(app, text="Connect", command=switch_to_second_page, corner_radius=15, font=("Ubuntu", 20, "bold"), width=100, height=70)
    submit_button.pack(pady=20, expand=True)

def second_page():
    print(f"Port: {port_entry.get()}")
    print(f"IP: {ip_entry.get()}")
    print(f"Index IP: {index_ip_entry.get()}")
    print(f"Index Port: {index_port_entry.get()}")
    print(f"Directory: {directory_entry.get()}")
    
    customtkinter.set_appearance_mode("System")
    customtkinter.set_default_color_theme("green")

    def search_file_page():
        filename = filename_entry.get()
        if filename:
            search_file(port, indexIp, indexPort, filename)
            print(f"Searching for {filename} in the network...") 
        else:
            messagebox.showwarning("Empty Field", "Please enter a file name.")

    app.title("Search File Page")
    app.geometry("450x620")

    filename_entry = customtkinter.CTkEntry(app, placeholder_text="File name", corner_radius=15, font=("Ubuntu", 20), width=350, height=200)
    filename_entry.pack(pady=20, expand=True)

    search_button = customtkinter.CTkButton(app, text="Search File in the Network", command=search_file_page, corner_radius=15, font=("Ubuntu", 14, "bold"), height=180, width=300)
    search_button.pack(pady=20, expand=True)

# Main application window
app = customtkinter.CTk()
app.geometry("450x620")  
initial_page()
app.mainloop()
