%w( clang gcc9 ).each do |dir|
  Dir.chdir(dir) do
    puts( %x( cmake . 2>&1 ) )
    puts( %x( make 2>&1 ) )
    puts dir
    puts( %x( ./release/sample.out ) )
  end
end
