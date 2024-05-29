import os
import cgi

def main():
	# Get the query string from the environment variable
	query_string = os.getenv('QUERY_STRING')
	
	# Parse the query string
	form = cgi.FieldStorage()
	
	# Extract the 'name' and 'surname' parameters
	name = form.getvalue('name', 'Stranger')
	surname = form.getvalue('surname', '')
	# Print the HTTP headers
	print("Content-Type: text/html")
	print()  # End of headers
	
	# Print the HTML content
	print("<html>")
	print("<head><title>Hello</title></head>")
	print("<body>")
	print("<h1>Hello, {} {}</h1>".format(name, surname))
	# for name, value in os.environ.items():
	# 	print("{0}: {1}".format(name, value))
	# print(os.environ)
	print("</body>")
	print("</html>")

if __name__ == "__main__":
	main()
