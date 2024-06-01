import os
import sys
from urllib.parse import parse_qs

def main():
	try:
		content_length = int(os.environ.get('CONTENT_LENGTH', 0))
	except ValueError:
		content_length = 0
	input_data = sys.stdin.read(content_length)
	form_data = parse_qs(input_data)
	errors = []
	name = form_data.get('name', [''])[0].strip()
	email = form_data.get('email', [''])[0].strip()
	if not name:
		errors.append("Name is required.")
	if not email:
		errors.append("Colour is required.")
	if errors:
		response_body = "<html><body><h1>Errors</h1><ul>"
		for error in errors:
			response_body += f"<li>{error}</li>"
		response_body += "</ul></body></html>"
	else:
		response_body = f"<html><body><h1>Data Received</h1><p>Name: {name}</p><p>email: {email}</p>"
	print()
	print(response_body)

if __name__ == "__main__":
	main()
