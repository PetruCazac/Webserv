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
	colour = form_data.get('colour', [''])[0].strip()
	age = form_data.get('age', [''])[0].strip()
	if not name:
		errors.append("Name is required.")
	if not colour:
		errors.append("Colour is required.")
	if not age.isdigit():
		errors.append("Age must be a number.")
	else:
		age = int(age)
		if age < 0:
			errors.append("Age must be a positive number.")
	if errors:
		response_body = "<html><body><h1>Errors</h1><ul>"
		for error in errors:
			response_body += f"<li>{error}</li>"
		response_body += "</ul></body></html>"
	else:
		response_body = f"<html><body><h1>Data Received</h1><p>Name: {name}</p><p>colour: {colour}</p><p>Age: {age}</p></body></html>"
	print()
	print(response_body)

if __name__ == "__main__":
	main()
