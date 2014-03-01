echo "Copying entityx files"

rsync -arvh --progress \
--exclude "*_test.cc" \
--exclude "python" \
--exclude "Manager.*" \
"/Users/davidwicks/Code/entityx/entityx/" "../src/entityx"

echo "May copy Pockets sources in the future."
echo "Done"
