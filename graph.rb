require "pp"

list = File.open( "main.cpp", &:to_a ).map{ |line| 
  m=/DERIVE\((\w+)\,\s*(\w+)\,\s*__LINE__\)/.match(line)
  m && [ m[1], m[2] ]
}.compact


File.open( "graph.dot", "w" ) do |f|
  f.puts(<<~DIGRAPH )
    digraph G {
      graph[
        dpi=300,
        rankdir = BT,
        margin = 0,
      ];
      node [
        shape = box,
        margin = 0.03,
        height = 0.2,
      ];
    DIGRAPH
  list.each do |base, deri|
    f.puts( "#{deri} -> #{base};" )
  end
  f.puts("}")
end

puts( %x( dot -Tpng graph.dot -o graph.png ) )