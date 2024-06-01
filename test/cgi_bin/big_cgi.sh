#!/bin/sh
echo ""
echo "<html><body><h1>Starting...</h1></body></html>"
echo "<html><body><h1>Finished</h1>"
for i in {1..1000}
do
    echo "<p>This is line $i</p>"
done
echo "</body></html>"
