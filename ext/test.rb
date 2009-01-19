require 'evolutionary'

class Deck  
  def initialize
    @stack1 = [1,2,3,4,5]
    @stack2 = [6,7,8,9,10]
  end
  
  def encode
    genome = ""
    (1..10).each do |card|
      if @stack1.include?(card)
        genome << "0"
      else
        genome << "1"
      end
    end
    return genome
  end
  
  def decode(genome)
    n=1

    @stack1 = []
    @stack2 = []
    
    genome.each_char do |char|
      if char == "0"
        @stack1 << n.to_i
      elsif char == "1"
        @stack2 << n.to_i
      end
      n += 1
    end        
  end
  
  def fitness
    sum = @stack1.inject(0){ |a, b| a += b }
    error1 = (36 - sum).abs
    
    product = @stack2.inject(1){ |a, b| a *= b }
    error2 = (360 - product).abs
    
    return -(error1 + error2)
  end
end

class MyGA < Evolutionary::GeneticAlgorithm  
end

deck = Deck.new
ga = MyGA.new(deck)

ga.evolve(20000, 20)