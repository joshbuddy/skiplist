require 'rake'
require 'spec'
require 'spec/rake/spectask'
require 'rake/extensiontask'

Spec::Rake::SpecTask.new do |t|
  t.spec_opts ||= []
  t.spec_opts << "-rubygems"
  t.spec_opts << "--options" << "spec/spec.opts"
  t.spec_files = FileList['spec/*_spec.rb']
end

Rake::ExtensionTask.new('cskiplist')
