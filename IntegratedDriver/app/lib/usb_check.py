import usb

if __name__ == "__main__":
    print(dict(usb.core.find(find_all=True)))
